#include "q_shared.h"
#include "cm_public.h"

qboolean CM_TraceBoxInternal(TraceExtents *extents, const float *testvec, float s, float *fraction)
{
    int i;
    float d, o;
    
    for(i = 0; i < 3; ++i)
    {
        d = (extents->start[i] - testvec[i]) * s;
        o = (extents->end[i] - testvec[i]) * s;
        if ( d <= 0.0 )
        {
            if ( o > 0.0 )
            {
                d = (d * extents->invDelta[i]) * s;
                if ( d <= 0.0 )
                    return 1;
                if ( d - *fraction < 0.0 )
                    *fraction = d;
            }
        }
        else if ( o > 0.0 || ((float)(d * extents->invDelta[i]) * s) >= *fraction )
            return 1;
    }
    return 0;
}

qboolean CM_TraceBox(TraceExtents *extents, const float *mins, const float *maxs, float fraction)
{
    float lfraction = fraction;

    if(CM_TraceBoxInternal(extents, mins, -1.0, &lfraction) == qtrue)
    {
        return qtrue;
    }
    if(CM_TraceBoxInternal(extents, maxs, 1.0, &lfraction) == qtrue)
    {
        return qtrue;
    }
    return qfalse;
}


void CM_CalcTraceExtents(TraceExtents *extents)
{
    int i;
    float diff;

    for ( i = 0; i < 3; ++i )
    {
        diff = extents->start[i] - extents->end[i];
        if ( diff != 0.0 )
            extents->invDelta[i] = 1.0 / diff;
        else extents->invDelta[i] = 0.0;
    }
}


