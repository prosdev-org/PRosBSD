#ifndef SHELL_FS_HXX
#define SHELL_FS_HXX

#include <shell/command.hxx>

namespace Shell::Fs {
    Command get_mkdir_command();
    Command get_touch_command();
    Command get_cat_command();
    Command get_write_command();
    Command get_rm_command();
    Command get_rmdir_command();
    Command get_mv_command();
    Command get_cd_command();
    Command get_pwd_command();
    Command get_ls_command();
    Command get_stat_command();
} // namespace Shell::Fs

#endif // SHELL_FS_HXX
