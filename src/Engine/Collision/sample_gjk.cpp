#include "gjk.h"
#include <assert.h>

#define GJK_FLT_MAX 3.40282347E+38F
#define GJK_EPSILON 1.19209290E-07F

static const float f3z[3];
#define fop(r,e,a,p,b,i,s) (r) e ((a) p (b)) i (s)
#define f3op(r,e,a,p,b,i,s) do {\
    fop((r)[0],e,(a)[0],p,(b)[0],i,s),\
    fop((r)[1],e,(a)[1],p,(b)[1],i,s),\
    fop((r)[2],e,(a)[2],p,(b)[2],i,s);}while(0)
#define f3cpy(d,s) (d)[0]=(s)[0],(d)[1]=(s)[1],(d)[2]=(s)[2]
#define f3add(d,a,b) f3op(d,=,a,+,b,+,0)
#define f3sub(d,a,b) f3op(d,=,a,-,b,+,0)
#define f3mul(d,a,s) f3op(d,=,a,+,f3z,*,s)
#define f3dot(a,b) ((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define f3cross(d,a,b) do {\
    (d)[0] = ((a)[1]*(b)[2]) - ((a)[2]*(b)[1]),\
    (d)[1] = ((a)[2]*(b)[0]) - ((a)[0]*(b)[2]),\
    (d)[2] = ((a)[0]*(b)[1]) - ((a)[1]*(b)[0]);}while(0)

static inline float
f3box(const float *a, const float *b, const float *c)
{
    float n[3];
    f3cross(n, a, b);
    return f3dot(n, c);
}
static float
inv_sqrt(float n)
{
    union {unsigned u; float f;} conv; conv.f = n;
    conv.u = 0x5f375A84 - (conv.u >> 1);
    conv.f = conv.f * (1.5f - (n * 0.5f * conv.f * conv.f));
    return conv.f;
}
extern int
gjk(struct gjk_simplex *s, const struct gjk_support *sup, float *dv)
{
    assert(s);
    assert(dv);
    assert(sup);
    if (!s || !sup || !dv) return 0;
    if (s->max_iter > 0 && s->iter >= s->max_iter)
        return 0;

    /* I.) Initialize */
    if (s->cnt == 0) {
        s->D = GJK_FLT_MAX;
        s->max_iter = !s->max_iter ? GJK_MAX_ITERATIONS: s->max_iter;
    }
    /* II.) Check for duplications */
    for (int i = 0; i < s->cnt; ++i) {
        if (sup->aid != s->v[i].aid) continue;
        if (sup->bid != s->v[i].bid) continue;
        return 0;
    }
    /* III.) Add vertex into simplex */
    struct gjk_vertex *vert = &s->v[s->cnt];
    f3cpy(vert->a, sup->a);
    f3cpy(vert->b, sup->b);
    f3cpy(vert->p, dv);
    vert->aid = sup->aid;
    vert->bid = sup->bid;
    s->bc[s->cnt++] = 1.0f;

    /* IV.) Find closest simplex point */
    switch (s->cnt) {
    case 1: break;
    case 2: {
        /* -------------------- Line ----------------------- */
        float a[3]; f3cpy(a, s->v[0].p);
        float b[3]; f3cpy(b, s->v[1].p);

        /* compute barycentric coordinates */
        float ab[3]; f3sub(ab, a, b);
        float ba[3]; f3sub(ba, b, a);

        float u = f3dot(b, ba);
        float v = f3dot(a, ab);
        if (v <= 0.0f) {
            /* region A */
            s->bc[0] = 1.0f;
            s->cnt = 1;
            break;
        }
        if (u <= 0.0f) {
            /* region B */
            s->v[0] = s->v[1];
            s->bc[0] = 1.0f;
            s->cnt = 1;
            break;
        }
        /* region AB */
        s->bc[0] = u;
        s->bc[1] = v;
        s->cnt = 2;
    } break;
    case 3: {
        /* -------------------- Triangle ----------------------- */
        float a[3]; f3cpy(a, s->v[0].p);
        float b[3]; f3cpy(b, s->v[1].p);
        float c[3]; f3cpy(c, s->v[2].p);

        float ab[3]; f3sub(ab, a, b);
        float ba[3]; f3sub(ba, b, a);
        float bc[3]; f3sub(bc, b, c);
        float cb[3]; f3sub(cb, c, b);
        float ca[3]; f3sub(ca, c, a);
        float ac[3]; f3sub(ac, a, c);

        /* compute barycentric coordinates */
        float u_ab = f3dot(b, ba);
        float v_ab = f3dot(a, ab);

        float u_bc = f3dot(c, cb);
        float v_bc = f3dot(b, bc);

        float u_ca = f3dot(a, ac);
        float v_ca = f3dot(c, ca);

        if (v_ab <= 0.0f && u_ca <= 0.0f) {
            /* region A */
            s->bc[0] = 1.0f;
            s->cnt = 1;
            break;
        }
        if (u_ab <= 0.0f && v_bc <= 0.0f) {
            /* region B */
            s->v[0] = s->v[1];
            s->bc[0] = 1.0f;
            s->cnt = 1;
            break;
        }
        if (u_bc <= 0.0f && v_ca <= 0.0f) {
            /* region C */
            s->v[0] = s->v[2];
            s->bc[0] = 1.0f;
            s->cnt = 1;
            break;
        }
        /* calculate fractional area */
        float n[3]; f3cross(n, ba, ca);
        float n1[3]; f3cross(n1, b, c);
        float n2[3]; f3cross(n2, c, a);
        float n3[3]; f3cross(n3, a, b);

        float u_abc = f3dot(n1, n);
        float v_abc = f3dot(n2, n);
        float w_abc = f3dot(n3, n);

        if (u_ab > 0.0f && v_ab > 0.0f && w_abc <= 0.0f) {
            /* region AB */
            s->bc[0] = u_ab;
            s->bc[1] = v_ab;
            s->cnt = 2;
            break;
        }
        if (u_bc > 0.0f && v_bc > 0.0f && u_abc <= 0.0f) {
            /* region BC */
            s->v[0] = s->v[1];
            s->v[1] = s->v[2];
            s->bc[0] = u_bc;
            s->bc[1] = v_bc;
            s->cnt = 2;
            break;
        }
        if (u_ca > 0.0f && v_ca > 0.0f && v_abc <= 0.0f) {
            /* region CA */
            s->v[1] = s->v[0];
            s->v[0] = s->v[2];
            s->bc[0] = u_ca;
            s->bc[1] = v_ca;
            s->cnt = 2;
            break;
        }
        /* region ABC */
        assert(u_abc > 0.0f && v_abc > 0.0f && w_abc > 0.0f);
        s->bc[0] = u_abc;
        s->bc[1] = v_abc;
        s->bc[2] = w_abc;
        s->cnt = 3;
    } break;
    case 4: {
        /* -------------------- Tetrahedron ----------------------- */
        float a[3]; f3cpy(a, s->v[0].p);
        float b[3]; f3cpy(b, s->v[1].p);
        float c[3]; f3cpy(c, s->v[2].p);
        float d[3]; f3cpy(d, s->v[3].p);

        float ab[3]; f3sub(ab, a, b);
        float ba[3]; f3sub(ba, b, a);
        float bc[3]; f3sub(bc, b, c);
        float cb[3]; f3sub(cb, c, b);
        float ca[3]; f3sub(ca, c, a);
        float ac[3]; f3sub(ac, a, c);

        float db[3]; f3sub(db, d, b);
        float bd[3]; f3sub(bd, b, d);
        float dc[3]; f3sub(dc, d, c);
        float cd[3]; f3sub(cd, c, d);
        float da[3]; f3sub(da, d, a);
        float ad[3]; f3sub(ad, a, d);

        /* compute barycentric coordinates */
        float u_ab = f3dot(b, ba);
        float v_ab = f3dot(a, ab);

        float u_bc = f3dot(c, cb);
        float v_bc = f3dot(b, bc);

        float u_ca = f3dot(a, ac);
        float v_ca = f3dot(c, ca);

        float u_bd = f3dot(d, db);
        float v_bd = f3dot(b, bd);

        float u_dc = f3dot(c, cd);
        float v_dc = f3dot(d, dc);

        float u_ad = f3dot(d, da);
        float v_ad = f3dot(a, ad);

        /* check verticies for closest point */
        if (v_ab <= 0.0f && u_ca <= 0.0f && v_ad <= 0.0f) {
            /* region A */
            s->bc[0] = 1.0f;
            s->cnt = 1;
            break;
        }
        if (u_ab <= 0.0f && v_bc <= 0.0f && v_bd <= 0.0f) {
            /* region B */
            s->v[0] = s->v[1];
            s->bc[0] = 1.0f;
            s->cnt = 1;
            break;
        }
        if (u_bc <= 0.0f && v_ca <= 0.0f && u_dc <= 0.0f) {
            /* region C */
            s->v[0] = s->v[2];
            s->bc[0] = 1.0f;
            s->cnt = 1;
            break;
        }
        if (u_bd <= 0.0f && v_dc <= 0.0f && u_ad <= 0.0f) {
            /* region D */
            s->v[0] = s->v[3];
            s->bc[0] = 1.0f;
            s->cnt = 1;
            break;
        }
        /* calculate fractional area */
        float n[3]; f3cross(n, da, ba);
        float n1[3]; f3cross(n1, d, b);
        float n2[3]; f3cross(n2, b, a);
        float n3[3]; f3cross(n3, a, d);

        float u_adb = f3dot(n1, n);
        float v_adb = f3dot(n2, n);
        float w_adb = f3dot(n3, n);

        f3cross(n, ca, da);
        f3cross(n1, c, d);
        f3cross(n2, d, a);
        f3cross(n3, a, c);

        float u_acd = f3dot(n1, n);
        float v_acd = f3dot(n2, n);
        float w_acd = f3dot(n3, n);

        f3cross(n, bc, dc);
        f3cross(n1, b, d);
        f3cross(n2, d, c);
        f3cross(n3, c, b);

        float u_cbd = f3dot(n1, n);
        float v_cbd = f3dot(n2, n);
        float w_cbd = f3dot(n3, n);

        f3cross(n, ba, ca);
        f3cross(n1, b, c);
        f3cross(n2, c, a);
        f3cross(n3, a, b);

        float u_abc = f3dot(n1, n);
        float v_abc = f3dot(n2, n);
        float w_abc = f3dot(n3, n);

        /* check edges for closest point */
        if (w_abc <= 0.0f && v_adb <= 0.0f && u_ab > 0.0f && v_ab > 0.0f) {
            /* region AB */
            s->bc[0] = u_ab;
            s->bc[1] = v_ab;
            s->cnt = 2;
            break;
        }
        if (u_abc <= 0.0f && w_cbd <= 0.0f && u_bc > 0.0f && v_bc > 0.0f) {
            /* region BC */
            s->v[0] = s->v[1];
            s->v[1] = s->v[2];
            s->bc[0] = u_bc;
            s->bc[1] = v_bc;
            s->cnt = 2;
            break;
        }
        if (v_abc <= 0.0f && w_acd <= 0.0f && u_ca > 0.0f && v_ca > 0.0f) {
            /* region CA */
            s->v[1] = s->v[0];
            s->v[0] = s->v[2];
            s->bc[0] = u_ca;
            s->bc[1] = v_ca;
            s->cnt = 2;
            break;
        }
        if (v_cbd <= 0.0f && u_acd <= 0.0f && u_dc > 0.0f && v_dc > 0.0f) {
            /* region DC */
            s->v[0] = s->v[3];
            s->v[1] = s->v[2];
            s->bc[0] = u_dc;
            s->bc[1] = v_dc;
            s->cnt = 2;
            break;
        }
        if (v_acd <= 0.0f && w_adb <= 0.0f && u_ad > 0.0f && v_ad > 0.0f) {
            /* region AD */
            s->v[1] = s->v[3];
            s->bc[0] = u_ad;
            s->bc[1] = v_ad;
            s->cnt = 2;
            break;
        }
        if (u_cbd <= 0.0f && u_adb <= 0.0f && u_bd > 0.0f && v_bd > 0.0f) {
            /* region BD */
            s->v[0] = s->v[1];
            s->v[1] = s->v[3];
            s->bc[0] = u_bd;
            s->bc[1] = v_bd;
            s->cnt = 2;
            break;
        }
        /* calculate fractional volume (volume can be negative!) */
        float denom = f3box(cb, ab, db);
        float volume = (denom == 0) ? 1.0f: 1.0f/denom;
        float u_abcd = f3box(c, d, b) * volume;
        float v_abcd = f3box(c, a, d) * volume;
        float w_abcd = f3box(d, a, b) * volume;
        float x_abcd = f3box(b, a, c) * volume;

        /* check faces for closest point */
        if (x_abcd <= 0.0f && u_abc > 0.0f && v_abc > 0.0f && w_abc > 0.0f) {
            /* region ABC */
            s->bc[0] = u_abc;
            s->bc[1] = v_abc;
            s->bc[2] = w_abc;
            s->cnt = 3;
            break;
        }
        if (u_abcd <= 0.0f && u_cbd > 0.0f && v_cbd > 0.0f && w_cbd > 0.0f) {
            /* region CBD */
            s->v[0] = s->v[2];
            s->v[2] = s->v[3];
            s->bc[0] = u_cbd;
            s->bc[1] = v_cbd;
            s->bc[2] = w_cbd;
            s->cnt = 3;
            break;
        }
        if (v_abcd <= 0.0f && u_acd > 0.0f && v_acd > 0.0f && w_acd > 0.0f) {
            /* region ACD */
            s->v[1] = s->v[2];
            s->v[2] = s->v[3];
            s->bc[0] = u_acd;
            s->bc[1] = v_acd;
            s->bc[2] = w_acd;
            s->cnt = 3;
            break;
        }
        if (w_abcd <= 0.0f && u_adb > 0.0f && v_adb > 0.0f && w_adb > 0.0f) {
            /* region ADB */
            s->v[2] = s->v[1];
            s->v[1] = s->v[3];
            s->bc[0] = u_adb;
            s->bc[1] = v_adb;
            s->bc[2] = w_adb;
            s->cnt = 3;
            break;
        }
        /* region ABCD */
        assert(u_abcd > 0.0f && v_abcd > 0.0f && w_abcd > 0.0f && x_abcd > 0.0f);
        s->bc[0] = u_abcd;
        s->bc[1] = v_abcd;
        s->bc[2] = w_abcd;
        s->bc[3] = x_abcd;
        s->cnt = 4;
    } break;}

    /* V.) Check if origin is enclosed by tetrahedron */
    if (s->cnt == 4) {
        s->hit = 1;
        return 0;
    }
    /* VI.) Ensure closing in on origin to prevent multi-step cycling */
    float pnt[3], denom = 0;
    for (int i = 0; i < s->cnt; ++i)
        denom += s->bc[i];
    denom = 1.0f / denom;

    switch (s->cnt) {
    case 1: f3cpy(pnt, s->v[0].p); break;
    case 2: {
        /* --------- Line -------- */
        float a[3]; f3mul(a, s->v[0].p, denom * s->bc[0]);
        float b[3]; f3mul(b, s->v[1].p, denom * s->bc[1]);
        f3add(pnt, a, b);
    } break;
    case 3: {
        /* ------- Triangle ------ */
        float a[3]; f3mul(a, s->v[0].p, denom * s->bc[0]);
        float b[3]; f3mul(b, s->v[1].p, denom * s->bc[1]);
        float c[3]; f3mul(c, s->v[2].p, denom * s->bc[2]);

        f3add(pnt, a, b);
        f3add(pnt, pnt, c);
    } break;
    case 4: {
        /* ----- Tetrahedron ----- */
        float a[3]; f3mul(a, s->v[0].p, denom * s->bc[0]);
        float b[3]; f3mul(b, s->v[1].p, denom * s->bc[1]);
        float c[3]; f3mul(c, s->v[2].p, denom * s->bc[2]);
        float d[3]; f3mul(d, s->v[3].p, denom * s->bc[3]);

        f3add(pnt, a, b);
        f3add(pnt, pnt, c);
        f3add(pnt, pnt, d);
    } break;}

    float d2 = f3dot(pnt, pnt);
    if (d2 >= s->D) return 0;
    s->D = d2;

    /* VII.) New search direction */
    switch (s->cnt) {
    default: assert(0); break;
    case 1: {
        /* --------- Point -------- */
        f3mul(dv, s->v[0].p, -1);
    } break;
    case 2: {
        /* ------ Line segment ---- */
        float ba[3]; f3sub(ba, s->v[1].p, s->v[0].p);
        float b0[3]; f3mul(b0, s->v[1].p, -1);
        float t[3];  f3cross(t, ba, b0);
        f3cross(dv, t, ba);
    } break;
    case 3: {
        /* ------- Triangle ------- */
        float ab[3]; f3sub(ab, s->v[1].p, s->v[0].p);
        float ac[3]; f3sub(ac, s->v[2].p, s->v[0].p);
        float n[3];  f3cross(n, ab, ac);
        if (f3dot(n, s->v[0].p) <= 0.0f)
            f3cpy(dv, n);
        else f3mul(dv, n, -1);
    }}
    if (f3dot(dv,dv) < GJK_EPSILON * GJK_EPSILON)
        return 0;
    return 1;
}
extern void
gjk_analyze(struct gjk_result *res, const struct gjk_simplex *s)
{
    res->iterations = s->iter;
    res->hit = s->hit;

    /* calculate normalization denominator */
    float denom = 0;
    for (int i = 0; i < s->cnt; ++i)
        denom += s->bc[i];
    denom = 1.0f / denom;

    /* compute closest points */
    switch (s->cnt) {
    default: assert(0); break;
    case 1: {
        /* Point */
        f3cpy(res->p0, s->v[0].a);
        f3cpy(res->p1, s->v[0].b);
    } break;
    case 2: {
        /* Line */
        float as = denom * s->bc[0];
        float bs = denom * s->bc[1];

        float a[3]; f3mul(a, s->v[0].a, as);
        float b[3]; f3mul(b, s->v[1].a, bs);
        float c[3]; f3mul(c, s->v[0].b, as);
        float d[3]; f3mul(d, s->v[1].b, bs);

        f3add(res->p0, a, b);
        f3add(res->p1, c, d);
    } break;
    case 3: {
        /* Triangle */
        float as = denom * s->bc[0];
        float bs = denom * s->bc[1];
        float cs = denom * s->bc[2];

        float a[3]; f3mul(a, s->v[0].a, as);
        float b[3]; f3mul(b, s->v[1].a, bs);
        float c[3]; f3mul(c, s->v[2].a, cs);

        float d[3]; f3mul(d, s->v[0].b, as);
        float e[3]; f3mul(e, s->v[1].b, bs);
        float f[3]; f3mul(f, s->v[2].b, cs);

        f3add(res->p0, a, b);
        f3add(res->p0, res->p0, c);

        f3add(res->p1, d, e);
        f3add(res->p1, res->p1, f);
    } break;
    case 4: {
        /* Tetrahedron */
        float a[3]; f3mul(a, s->v[0].a, denom * s->bc[0]);
        float b[3]; f3mul(b, s->v[1].a, denom * s->bc[1]);
        float c[3]; f3mul(c, s->v[2].a, denom * s->bc[2]);
        float d[3]; f3mul(d, s->v[3].a, denom * s->bc[3]);

        f3add(res->p0, a, b);
        f3add(res->p0, res->p0, c);
        f3add(res->p0, res->p0, d);
        f3cpy(res->p1, res->p0);
    } break;}

    if (!res->hit) {
        /* compute distance */
        float d[3]; f3sub(d, res->p1, res->p0);
        res->distance_squared = f3dot(d, d);
    } else res->distance_squared = 0;
}
extern void
gjk_quad(struct gjk_result *res, float a_radius, float b_radius)
{
    float radius = a_radius + b_radius;
    float radius_squared = radius * radius;
    if (res->distance_squared > GJK_EPSILON &&
        res->distance_squared > radius_squared) {
        res->distance_squared -= radius_squared;

        /* calculate normal */
        float n[3]; f3sub(n, res->p1, res->p0);
        float l2 = f3dot(n, n);
        if (l2 != 0.0f) {
            float il = inv_sqrt(l2);
            f3mul(n,n,il);
        }
        float da[3]; f3mul(da, n, a_radius);
        float db[3]; f3mul(db, n, b_radius);

        /* calculate new collision points */
        f3add(res->p0, res->p0, da);
        f3sub(res->p1, res->p1, db);
    } else {
        float p[3]; f3add(p, res->p0, res->p1);
        f3mul(res->p0, p, 0.5f);
        f3cpy(res->p1, res->p0);
        res->distance_squared = 0;
        res->hit = 1;
    }
}