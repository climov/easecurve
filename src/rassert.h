#ifndef RASSERT_H
#define RASSERT_H

#define R_ASSERT(cond) (((cond)) ? r_noop() : r_assert(#cond,__FILE__,__LINE__))
[[noreturn]] void r_assert(const char*, const char*, int);
inline void r_noop() noexcept {}

#endif // RASSERT_H
