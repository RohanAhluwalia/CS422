#ifdef DEBUG_MSG

#include <dev/console.h>
#include <dev/serial.h>

#include <lib/debug.h>
#include <lib/stdarg.h>
#include <lib/spinlock.h>

struct dprintbuf {
    int idx;  /* current buffer index */
    int cnt;  /* total bytes printed so far */
    char buf[CONSOLE_BUFFER_SIZE];
};

extern spinlock_t cons_lock;

static void cputs(const char *str)
{
    while (*str) {
        cons_putc(*str);
        str += 1;
    }
}

static void putch(int ch, struct dprintbuf *b)
{
    b->buf[b->idx++] = ch;
    if (b->idx == CONSOLE_BUFFER_SIZE - 1) {
        b->buf[b->idx] = 0;
        cputs(b->buf);
        b->idx = 0;
    }
    b->cnt++;
}

int vdprintf(const char *fmt, va_list ap)
{
    struct dprintbuf b;

    spinlock_acquire(&cons_lock);

    b.idx = 0;
    b.cnt = 0;
    vprintfmt((void *) putch, &b, fmt, ap);

    b.buf[b.idx] = 0;
    cputs(b.buf);
    spinlock_release(&cons_lock);

    return b.cnt;
}

int dprintf(const char *fmt, ...)
{
    va_list ap;
    int cnt;

    va_start(ap, fmt);
    cnt = vdprintf(fmt, ap);
    va_end(ap);

    return cnt;
}

#endif  /* DEBUG_MSG */
