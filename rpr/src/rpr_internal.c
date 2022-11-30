/**
 * This special source file fix a compilation error related to FPU when not using CRT.
 *
 * <p>
 * It has no associated header.
 * </p>
 */

#if defined(__cplusplus)
extern "C" {
#endif
void *_fltused;
#if defined(__cplusplus)
};
#endif
