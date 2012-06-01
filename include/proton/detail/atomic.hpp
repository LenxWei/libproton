#ifndef PROTON_ACTOMIC_HEADER
#define PROTON_ACTOMIC_HEADER

namespace proton
{

namespace detail
{

class atomic_count
{
public:

    explicit atomic_count( long v ) : value_( v ) {}

    long operator++()
    {
        return atomic_exchange_and_add( &value_, +1 ) + 1;
    }

    long operator--()
    {
        return atomic_exchange_and_add( &value_, -1 ) - 1;
    }

    operator long() const
    {
        return atomic_exchange_and_add( &value_, 0 );
    }

private:

    atomic_count(atomic_count const &);
    atomic_count & operator=(atomic_count const &);

    mutable long value_;

private:

    static long atomic_inc( long * pw, long dv )
    {
        // int r = *pw;
        // *pw += dv;
        // return r;

        long r;

        __asm__ __volatile__
        (
            "lock\n\t"
            "xadd %1, %0":
            "+m"( *pw ), "=r"( r ): // outputs (%0, %1)
            "1"( dv ): // inputs (%2 == %1)
            "memory", "cc" // clobbers
        );

        return r;
    }
};

} // namespace detail

} // namespace proton

#endif // PROTON_ACTOMIC_HEADER
