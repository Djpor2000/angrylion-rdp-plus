static void vi_fetch_filter16(struct ccvg* res, uint32_t fboffset, uint32_t cur_x, uint32_t fsaa, uint32_t dither_filter, uint32_t vres, uint32_t fetchstate);
static void vi_fetch_filter32(struct ccvg* res, uint32_t fboffset, uint32_t cur_x, uint32_t fsaa, uint32_t dither_filter, uint32_t vres, uint32_t fetchstate);

static void (*vi_fetch_filter_func[2])(struct ccvg*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) =
{
    vi_fetch_filter16, vi_fetch_filter32
};

void (*vi_fetch_filter_ptr)(struct ccvg*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

static void vi_fetch_filter16(struct ccvg* res, uint32_t fboffset, uint32_t cur_x, uint32_t fsaa, uint32_t dither_filter, uint32_t vres, uint32_t fetchstate)
{
    int r, g, b;
    uint32_t idx = (fboffset >> 1) + cur_x;
    uint8_t hval;
    uint16_t pix;
    uint32_t cur_cvg;
    if (fsaa)
    {
        PAIRREAD16(pix, hval, idx);
        cur_cvg = ((pix & 1) << 2) | hval;
    }
    else
    {
        RREADIDX16(pix, idx);
        cur_cvg = 7;
    }
    r = GET_HI(pix);
    g = GET_MED(pix);
    b = GET_LOW(pix);

    uint32_t fbw = *vi_reg_ptr[VI_WIDTH] & 0xfff;

    if (cur_cvg == 7)
    {
        if (dither_filter)
            restore_filter16(&r, &g, &b, fboffset, cur_x, fbw, fetchstate);
    }
    else
    {
        video_filter16(&r, &g, &b, fboffset, cur_x, fbw, cur_cvg, fetchstate);
    }


    res->r = r;
    res->g = g;
    res->b = b;
    res->cvg = cur_cvg;
}

static void vi_fetch_filter32(struct ccvg* res, uint32_t fboffset, uint32_t cur_x, uint32_t fsaa, uint32_t dither_filter, uint32_t vres, uint32_t fetchstate)
{
    int r, g, b;
    uint32_t pix, addr = (fboffset >> 2) + cur_x;
    RREADIDX32(pix, addr);
    uint32_t cur_cvg;
    if (fsaa)
        cur_cvg = (pix >> 5) & 7;
    else
        cur_cvg = 7;
    r = (pix >> 24) & 0xff;
    g = (pix >> 16) & 0xff;
    b = (pix >> 8) & 0xff;

    uint32_t fbw = *vi_reg_ptr[VI_WIDTH] & 0xfff;

    if (cur_cvg == 7)
    {
        if (dither_filter)
            restore_filter32(&r, &g, &b, fboffset, cur_x, fbw, fetchstate);
    }
    else
    {
        video_filter32(&r, &g, &b, fboffset, cur_x, fbw, cur_cvg, fetchstate);
    }

    res->r = r;
    res->g = g;
    res->b = b;
    res->cvg = cur_cvg;
}
