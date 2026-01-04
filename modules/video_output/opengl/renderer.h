/*****************************************************************************
 * renderer.h: OpenGL internal header
 *****************************************************************************
 * Copyright (C) 2016 VLC authors and VideoLAN
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifndef VLC_GL_RENDERER_H
#define VLC_GL_RENDERER_H

#include <vlc_common.h>
#include <vlc_codec.h>
#include <vlc_opengl.h>
#include <vlc_opengl_filter.h>
#include <vlc_plugin.h>

#include "gl_api.h"
#include "gl_common.h"
#include "interop.h"
#include "sampler.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define add_opengl_submodule_renderer() \
    add_submodule() \
    add_shortcut("renderer") \
    set_shortname("renderer") \
    set_capability("opengl filter", 0) \
    set_callback_opengl_filter(vlc_gl_renderer_Open)

/**
 * OpenGL picture renderer
 */
struct vlc_gl_renderer
{
    /* Set by the caller */
    const struct vlc_gl_api *api;
    const opengl_vtable_t *vt; /* for convenience, same as &api->vt */

    /* True to dump shaders */
    bool dump_shaders;

    GLuint program_id;

    struct {
        GLfloat ProjectionMatrix[16];
        GLfloat StereoMatrix[3*3];
        GLfloat ZoomMatrix[16];
        GLfloat ViewMatrix[16];
    } var;

    struct {
        GLint StereoMatrix;
        GLint ProjectionMatrix;
        GLint ViewMatrix;
        GLint ZoomMatrix;
        GLint OrientationMatrix;
    } uloc;

    struct {
        GLint PicCoordsIn;
        GLint VertexPosition;
    } aloc;

    struct vlc_gl_sampler *sampler;

    unsigned nb_indices;
    GLuint vertex_buffer_object;
    GLuint index_buffer_object;
    GLuint texture_buffer_object;

    GLuint vertex_array_object;

    bool valid_coords;

    /* View point */
    vlc_viewpoint_t vp;
    float f_teta;
    float f_phi;
    float f_roll;
    float f_fovx; /* f_fovx and f_fovy are linked but we keep both */
    float f_fovy; /* to avoid recalculating them when needed.      */
    float f_z;    /* Position of the camera on the sphere radius vector */
    float f_sar;

    /* Original size from which f_sar is computed
     *     f_sar = (float) target_width / target_height */
    unsigned target_width;
    unsigned target_height;

    /* Projection mode for the input data */
    video_projection_mode_t projection_mode;
    vlc_stereoscopic_mode_t stereo_mode;
    video_multiview_mode_t multiview_mode;

    /* VR-specific parameters */
    float f_ipd_offset;        /* IPD adjustment: -1.0 to 1.0 */
    float f_vr_zoom;           /* VR zoom level: 0.1 to 2.0 */
    bool b_vr_mode;            /* True if in VR mode (SBS equirectangular) */
};

vlc_gl_filter_open_fn vlc_gl_renderer_Open;

int
vlc_gl_renderer_SetViewpoint(struct vlc_gl_renderer *renderer,
                             const vlc_viewpoint_t *p_vp);

/**
 * Set IPD (interpupillary distance) adjustment for VR stereo rendering
 * \param renderer the renderer
 * \param ipd_offset adjustment from -1.0 (eyes closer) to 1.0 (eyes further)
 * \return VLC_SUCCESS or VLC_EINVAL if out of range
 */
int
vlc_gl_renderer_SetIPD(struct vlc_gl_renderer *renderer, float ipd_offset);

/**
 * Set zoom level for VR mode
 * \param renderer the renderer
 * \param zoom zoom factor (0.1 to 2.0, where 1.0 = 100%)
 * \return VLC_SUCCESS or VLC_EINVAL if out of range
 */
int
vlc_gl_renderer_SetVRZoom(struct vlc_gl_renderer *renderer, float zoom);

/**
 * Adjust VR zoom by a delta amount
 * \param renderer the renderer
 * \param delta amount to change zoom (positive = zoom in, negative = zoom out)
 * \return VLC_SUCCESS or VLC_EINVAL
 */
int
vlc_gl_renderer_AdjustVRZoom(struct vlc_gl_renderer *renderer, float delta);

/**
 * Adjust IPD by a delta amount
 * \param renderer the renderer
 * \param delta amount to change IPD
 * \return VLC_SUCCESS or VLC_EINVAL
 */
int
vlc_gl_renderer_AdjustIPD(struct vlc_gl_renderer *renderer, float delta);

#ifdef __cplusplus
}
#endif

#endif /* include-guard */