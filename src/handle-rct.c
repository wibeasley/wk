
#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>
#include "wk-v1.h"

#define REAL_NA(val) (ISNA(val) || ISNAN(val))

#define HANDLE_CONTINUE_OR_BREAK(expr)                         \
  result = expr;                                               \
  if (result == WK_ABORT_FEATURE) continue; else if (result == WK_ABORT) break

SEXP wk_read_rct(SEXP data, wk_handler_t* handler) {
    if (!Rf_inherits(data, "wk_rct")) {
        Rf_error("Object does not inherit from 'wk_rct'");
    }

    R_xlen_t n_features = Rf_xlength(VECTOR_ELT(data, 0));
    double* data_ptr[4];
    for (int j = 0; j < 4; j++) {
        data_ptr[j] = REAL(VECTOR_ELT(data, j));
    }

    wk_vector_meta_t vector_meta;
    WK_VECTOR_META_RESET(vector_meta, WK_POLYGON);
    vector_meta.size = n_features;

    if (handler->vector_start(&vector_meta, handler->handler_data) == WK_CONTINUE) {
        int result;
        double xmin, ymin, xmax, ymax;
        wk_coord_t coord;
        wk_meta_t meta;
        WK_META_RESET(meta, WK_POLYGON);
        meta.flags = vector_meta.flags | WK_FLAG_HAS_BOUNDS;

        for (R_xlen_t i = 0; i < n_features; i++) {
            if (((i + 1) % 1000) == 0) R_CheckUserInterrupt();
            
            HANDLE_CONTINUE_OR_BREAK(handler->feature_start(&vector_meta, i, handler->handler_data));

            xmin = data_ptr[0][i];
            ymin = data_ptr[1][i];
            xmax = data_ptr[2][i];
            ymax = data_ptr[3][i];
            int rect_na = REAL_NA(xmin) && REAL_NA(ymin) && REAL_NA(xmax) && REAL_NA(ymax);
            int rect_empty = rect_na || ((xmax - xmin) == R_NegInf) || ((ymax - ymin) == R_NegInf);

            if (rect_empty) {
                meta.size = 0;
            } else {
                meta.size = 1;
            }

            meta.bounds_min[0] = xmin;
            meta.bounds_min[1] = ymin;
            meta.bounds_max[0] = xmax;
            meta.bounds_max[1] = ymax;

            HANDLE_CONTINUE_OR_BREAK(handler->geometry_start(&meta, WK_PART_ID_NONE, handler->handler_data));
            if (!rect_empty) {
                HANDLE_CONTINUE_OR_BREAK(handler->ring_start(&meta, 5, 0, handler->handler_data));
                coord.v[0] = xmin; coord.v[1] = ymin;
                HANDLE_CONTINUE_OR_BREAK(handler->coord(&meta, coord, 0, handler->handler_data));
                coord.v[0] = xmax; coord.v[1] = ymin;
                HANDLE_CONTINUE_OR_BREAK(handler->coord(&meta, coord, 1, handler->handler_data));
                coord.v[0] = xmax; coord.v[1] = ymax;
                HANDLE_CONTINUE_OR_BREAK(handler->coord(&meta, coord, 2, handler->handler_data));
                coord.v[0] = xmin; coord.v[1] = ymax;
                HANDLE_CONTINUE_OR_BREAK(handler->coord(&meta, coord, 3, handler->handler_data));
                coord.v[0] = xmin; coord.v[1] = ymin;
                HANDLE_CONTINUE_OR_BREAK(handler->coord(&meta, coord, 4, handler->handler_data));
                HANDLE_CONTINUE_OR_BREAK(handler->ring_end(&meta, 5, 0, handler->handler_data));
            }
            HANDLE_CONTINUE_OR_BREAK(handler->geometry_end(&meta, WK_PART_ID_NONE, handler->handler_data));

            if (handler->feature_end(&vector_meta, i, handler->handler_data) == WK_ABORT) {
                break;
            }
        }
    }

    SEXP result = PROTECT(handler->vector_end(&vector_meta, handler->handler_data));
    UNPROTECT(1);
    return result;
}

SEXP wk_c_read_rct(SEXP data, SEXP handlerXptr) {
    return wk_handler_run_xptr(&wk_read_rct, data, handlerXptr);
}
