/**
 *  Copyright (c) 2013 Tatsuhiko Kubo <cubicdaiya@gmail.com>
 *  Copyright (c) ngx_mruby developers 2012-
 *
 *  Use and distribution licensed under the MIT license.
 *  See LICENSE for full text.
 *
 */

#include "ngx_http_mruby_module.h"
#include "ngx_http_mruby_class.h"

#include <mruby.h>
#include <mruby/class.h>
#include <mruby/string.h>

static mrb_value ngx_http_mruby_base64_encode(mrb_state *mrb, mrb_value self);
static mrb_value ngx_http_mruby_base64_decode(mrb_state *mrb, mrb_value self);

static mrb_value ngx_http_mruby_base64_encode(mrb_state *mrb, mrb_value self)
{
    mrb_value mrb_src;
    ngx_str_t src, dst;

    mrb_get_args(mrb, "o", &mrb_src);

    if (mrb_type(mrb_src) != MRB_TT_STRING) {
        mrb_src = mrb_funcall(mrb, mrb_src, "to_s", 0, NULL);
    }

    src.data = (u_char *)RSTRING_PTR(mrb_src);
    src.len  = RSTRING_LEN(mrb_src);

    dst.len  = ngx_base64_encoded_length(src.len);
    dst.data = mrb_malloc(mrb, dst.len + 1);

    ngx_encode_base64(&dst, &src);

    return mrb_str_new(mrb, (char *)dst.data, dst.len);
}

static mrb_value ngx_http_mruby_base64_decode(mrb_state *mrb, mrb_value self)
{
    mrb_value mrb_src;
    ngx_str_t src, dst;

    mrb_get_args(mrb, "o", &mrb_src);

    if (mrb_type(mrb_src) != MRB_TT_STRING) {
        mrb_src = mrb_funcall(mrb, mrb_src, "to_s", 0, NULL);
    }

    src.data = (u_char *)RSTRING_PTR(mrb_src);
    src.len  = RSTRING_LEN(mrb_src);

    dst.len  = ngx_base64_decoded_length(src.len);
    dst.data = mrb_malloc(mrb, dst.len + 1);

    if (ngx_decode_base64(&dst, &src) == NGX_ERROR) {
        return mrb_nil_value();
    }

    return mrb_str_new(mrb, (char *)dst.data, dst.len);
}

void ngx_http_mruby_base64_class_init(mrb_state *mrb, struct RClass *class)
{
    struct RClass *class_base64;

    class_base64 = mrb_define_class_under(mrb, class, "Base64", mrb->object_class);

    mrb_define_class_method(mrb, class_base64, "encode", ngx_http_mruby_base64_encode, ARGS_ANY());
    mrb_define_class_method(mrb, class_base64, "decode", ngx_http_mruby_base64_decode, ARGS_ANY());
}
