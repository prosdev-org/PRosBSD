# Contribution and development guidelines of PRosBSD

## General rules
- G1. Safeness > Performance > Features.
- G2. Only use safe code.
- G3. By default, all code is unsafe until otherwise is proved.
- G4. No undefined behaviour.
- G5. The code must be clear and easy to understand.
- G6. If it is difficult to understand the code, include comments explaining what the code does.
- G7. No LLM code.
- G8. Maintainers do take responsibility for code they merge.
- G9. Code has to be reviewed by at least two maintainers to be merged to `dev` branch.
- G10. Architecture of the project and its parts such as VFS, memory models, drivers must be documented.
- G11. Code style must be consistent with the project's formatting rules. (Do not applicable to third party code)
- G12. External API/ABI must be documented and versioned.

## Programming rules
- C1. No `goto` operator.
- C2. No recursion.
- C3. Non more than three levels of nesting.
- C4. No VLA.
- C5. Declare all data objects at the smallest possible level of scope
- C6. All code must compile without warnings
- C7. No `asm` not in the arch code.
- C8. There is only one accepted way to do operations such as memory allocation, null-checking, etc.

## Style
- S1. Code formatting must follow the project style configuration.
