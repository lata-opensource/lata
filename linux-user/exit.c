/*
 *  exit support for qemu
 *
 *  Copyright (c) 2018 Alex Bennée <alex.bennee@linaro.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, see <http://www.gnu.org/licenses/>.
 */
#include "qemu/osdep.h"
#include "accel/tcg/perf.h"
#include "gdbstub/syscalls.h"
#include "qemu.h"
#include "user-internals.h"
#include "qemu/plugin.h"
#ifdef CONFIG_GPROF
#include <sys/gmon.h>
#endif

#ifdef CONFIG_GCOV
extern void __gcov_dump(void);
#endif

void preexit_cleanup(CPUArchState *env, int code)
{
#ifdef CONFIG_GPROF
        _mcleanup();
#endif
#ifdef CONFIG_GCOV
        __gcov_dump();
#endif
        gdb_exit(code);
        qemu_plugin_user_exit();
        perf_exit();
#ifdef CONFIG_LATA_INDIRECT_JMP
        if (indirect_jmp_opt_profile) {
                long jr_cnt = thread_cpu->env_ptr->jr_cnt;
                long jr_hit = thread_cpu->env_ptr->jr_hit;
                qemu_log("jr_cnt:%ld, jr_hit:%ld, hit_ratio:%f\n", jr_cnt, jr_hit, (double)jr_hit / jr_cnt);
        }
        
#endif
}
