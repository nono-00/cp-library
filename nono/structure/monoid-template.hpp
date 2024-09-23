template <class U>
struct Monoid {
    using T = U;
    static T op(T lhs, T rhs);
    static T e();
};
template <class U>
struct ActMonoid {
    using T = U;
    using F = U;
    static T op(T lhs, T rhs);
    static T e();
    static T mapping(F lhs, T rhs);
    //  rhsは事前に適用されていたやつ
    //  lhsがこれから適用するやつ
    static F compososition(F lhs, F rhs);
    static F id();
};
template <class U>
struct BeatMonoid {
    struct ope;
    struct info;
    using T = info;
    using F = ope;
    static T op(T lhs, T rhs);
    static T e();
    static T map(F lhs, T rhs);
    static F compose(F lhs, F rhs);
    static F id();
};
