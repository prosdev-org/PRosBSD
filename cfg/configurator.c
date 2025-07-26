#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>

#define MAX_LINE_LEN 512
#define MAX_VAR_NAME 32
#define MAX_VALUE_LEN 128
#define CONFIG_FILE "./cfg/Makefile.header"

typedef struct {
    char name[MAX_VAR_NAME];
    char prompt[64];
    char value[MAX_VALUE_LEN];
    char prefix[MAX_LINE_LEN];
    char suffix[MAX_LINE_LEN];
    int found;
} ConfigVar;

void trim_trailing_whitespace(char *str) {
    if (str == NULL || *str == '\0') return;
    
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
}

int read_config(ConfigVar *vars, int count) {
    FILE *file = fopen(CONFIG_FILE, "r");
    if (!file) {
        perror("\nError opening config file");
        return 0;
    }

    char line[MAX_LINE_LEN];
    int line_num = 0;
    
    while (fgets(line, sizeof(line), file)) {
        line_num++;
        char *p = line;
        
        while (isspace(*p)) p++;
        if (*p == '\0') continue;
        
        if (*p == '#') continue;
        
        for (int i = 0; i < count; i++) {
            if (vars[i].found) continue;
            
            size_t name_len = strlen(vars[i].name);
            if (strncmp(p, vars[i].name, name_len) != 0) continue;
            
            char *after_name = p + name_len;
            if (*after_name != ' ' && *after_name != '\t' && *after_name != ':' && *after_name != '?' && *after_name != '=') {
                continue;
            }
            
            char *op_start = after_name;
            while (*op_start && isspace(*op_start)) op_start++;
            
            char *op_end = op_start;
            if (strncmp(op_start, ":=", 2) == 0) {
                op_end += 2;
            } else if (strncmp(op_start, "?=", 2) == 0) {
                op_end += 2;
            } else if (*op_start == '=') {
                op_end += 1;
            } else {
                continue;
            }
            
            char *val_start = op_end;
            while (*val_start && isspace(*val_start)) val_start++;
            if (*val_start == '\0') continue;
        
            char *val_end = val_start + strlen(val_start);
            char *comment = strchr(val_start, '#');
            if (comment) {
                val_end = comment;
                while (val_end > val_start && isspace(*(val_end-1))) val_end--;
            } else {
                while (val_end > val_start && isspace(*(val_end-1))) val_end--;
            }
            
            size_t prefix_len = val_start - line;
            strncpy(vars[i].prefix, line, prefix_len);
            vars[i].prefix[prefix_len] = '\0';
            
            size_t value_len = val_end - val_start;
            if (value_len >= MAX_VALUE_LEN) value_len = MAX_VALUE_LEN - 1;
            strncpy(vars[i].value, val_start, value_len);
            vars[i].value[value_len] = '\0';
            
            if (comment) {
                strncpy(vars[i].suffix, comment, MAX_LINE_LEN - 1);
            } else {
                vars[i].suffix[0] = '\0';
            }
            
            vars[i].found = 1;
            break;
        }
    }
    
    fclose(file);
    return 1;
}

int write_config(ConfigVar *vars, int count) {
    FILE *file = fopen(CONFIG_FILE, "r");
    if (!file) {
        perror("\nError opening config file for reading");
        return 0;
    }

    FILE *temp = fopen("temp_config", "w");
    if (!temp) {
        perror("\nError creating temp file");
        fclose(file);
        return 0;
    }

    char line[MAX_LINE_LEN];
    int changed = 0;
    
    while (fgets(line, sizeof(line), file)) {
        int replaced = 0;
        trim_trailing_whitespace(line);
        
        for (int i = 0; i < count; i++) {
            if (!vars[i].found) continue;
            
            if (strstr(line, vars[i].name) == line || 
                strstr(line, vars[i].name) == line + 1) {
                char new_line[MAX_LINE_LEN];
                if (vars[i].suffix[0] != '\0') {
                    snprintf(new_line, sizeof(new_line), "%s%s %s", 
                             vars[i].prefix, vars[i].value, vars[i].suffix);
                } else {
                    snprintf(new_line, sizeof(new_line), "%s%s", 
                             vars[i].prefix, vars[i].value);
                }
                fprintf(temp, "%s\n", new_line);
                replaced = 1;
                changed = 1;
                break;
            }
        }
        
        if (!replaced) {
            fprintf(temp, "%s\n", line);
        }
    }
    
    fclose(file);
    fclose(temp);
    
    if (changed) {
        remove(CONFIG_FILE);
        rename("temp_config", CONFIG_FILE);
    } else {
        remove("temp_config");
    }
    
    return 1;
}

int main() {
    ConfigVar vars[] = {
        {"ISGRUBQ", "Default bootloader ('Y' for grub, 'N' for syslinux)", "", "", "", 0},
        {"MEMDISK", "Memdisk", "", "", "", 0},
        {"GRUB_MOUNT", "GRUB mount point", "", "", "", 0},
    
        {"ASFLAGS", "AS flags", "", "", "", 0},
        {"LDFLAGS", "Linker flags", "", "", "", 0},
        {"CFLAGS", "C compiler flags", "", "", "", 0},
        {"OBJCOPYFLAGS", "Objcopy flags (convert to binary)", "", "", "", 0},

        {"AS", "AS program", "", "", "", 0},
        {"CC", "C compiler program", "", "", "", 0},
        {"LD", "Linker program", "", "", "", 0},
        {"OBJCOPY", "Objcopy program", "", "", "", 0},
        {"STRIP", "ELF stripping program", "", "", "", 0},
    
        {"VERSION_MAJOR", "Major version number", "", "", "", 0},
        {"VERSION_MINOR", "Minor version number", "", "", "", 0},
        {"VERSION_PATCH", "Patch version number", "", "", "", 0},
        {"VERSION_SUFFIX", "Suffix of version", "", "", "", 0},
    };
    
    const int var_count = sizeof(vars) / sizeof(vars[0]);
    
    if (!read_config(vars, var_count)) {
        return 1;
    }
    
    printf("Pros configuration utility. Press enter to use default value.\n");
    
    for (int i = 0; i < var_count; i++) {
        char input[MAX_VALUE_LEN];
        printf("%s [%s]: ", vars[i].prompt, vars[i].value);
        
        if (fgets(input, sizeof(input), stdin)) {
            size_t len = strlen(input);
            if (len > 0 && input[len - 1] == '\n') {
                input[len - 1] = '\0';
            }
            
            if (strlen(input) > 0) {
                strncpy(vars[i].value, input, MAX_VALUE_LEN - 1);
                vars[i].value[MAX_VALUE_LEN - 1] = '\0';
            }
        }
    }
    
    if (write_config(vars, var_count)) {
        printf("\nConfiguration updated successfully!\n");
    } else {
        printf("\nFailed to update configuration!\n");
    }
    
    return 0;
}