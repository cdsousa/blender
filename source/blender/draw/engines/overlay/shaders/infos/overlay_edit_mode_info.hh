/* SPDX-FileCopyrightText: 2023 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

#include "gpu_shader_create_info.hh"

GPU_SHADER_INTERFACE_INFO(overlay_edit_flat_color_iface, "").flat(Type::VEC4, "finalColor");
GPU_SHADER_INTERFACE_INFO(overlay_edit_smooth_color_iface, "").smooth(Type::VEC4, "finalColor");
GPU_SHADER_INTERFACE_INFO(overlay_edit_nopersp_color_iface, "")
    .no_perspective(Type::VEC4, "finalColor");

/* -------------------------------------------------------------------- */
/** \name Edit Mesh
 * \{ */

GPU_SHADER_CREATE_INFO(overlay_edit_mesh_common)
    .define("blender_srgb_to_framebuffer_space(a)", "a")
    .sampler(0, ImageType::DEPTH_2D, "depthTex")
    .fragment_out(0, Type::VEC4, "fragColor")
    .push_constant(Type::BOOL, "wireShading")
    .push_constant(Type::BOOL, "selectFace")
    .push_constant(Type::BOOL, "selectEdge")
    .push_constant(Type::FLOAT, "alpha")
    .push_constant(Type::FLOAT, "retopologyOffset")
    .push_constant(Type::IVEC4, "dataMask")
    .additional_info("draw_globals");

#ifdef WITH_METAL_BACKEND
GPU_SHADER_CREATE_INFO(overlay_edit_mesh_common_no_geom)
    .metal_backend_only(true)
    .define("blender_srgb_to_framebuffer_space(a)", "a")
    .sampler(0, ImageType::DEPTH_2D, "depthTex")
    .fragment_out(0, Type::VEC4, "fragColor")
    .push_constant(Type::BOOL, "wireShading")
    .push_constant(Type::BOOL, "selectFace")
    .push_constant(Type::BOOL, "selectEdge")
    .push_constant(Type::FLOAT, "alpha")
    .push_constant(Type::FLOAT, "retopologyOffset")
    .push_constant(Type::IVEC4, "dataMask")
    .vertex_source("overlay_edit_mesh_vert_no_geom.glsl")
    .additional_info("draw_globals");
#endif

GPU_SHADER_CREATE_INFO(overlay_edit_mesh_depth)
    .do_static_compilation(true)
    .vertex_in(0, Type::VEC3, "pos")
    .push_constant(Type::FLOAT, "retopologyOffset")
    .vertex_source("overlay_edit_mesh_depth_vert.glsl")
    .fragment_source("overlay_depth_only_frag.glsl")
    .additional_info("draw_mesh");

GPU_SHADER_CREATE_INFO(overlay_edit_mesh_depth_clipped)
    .do_static_compilation(true)
    .additional_info("overlay_edit_mesh_depth", "drw_clipped");

GPU_SHADER_INTERFACE_INFO(overlay_edit_mesh_vert_iface, "")
    .smooth(Type::VEC4, "finalColor")
    .smooth(Type::FLOAT, "vertexCrease");

GPU_SHADER_CREATE_INFO(overlay_edit_mesh_vert)
    .do_static_compilation(true)
    .builtins(BuiltinBits::POINT_SIZE)
    .define("VERT")
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_in(1, Type::UVEC4, "data")
    .vertex_in(2, Type::VEC3, "vnor")
    .vertex_source("overlay_edit_mesh_vert.glsl")
    .vertex_out(overlay_edit_mesh_vert_iface)
    .fragment_source("overlay_point_varying_color_frag.glsl")
    .additional_info("draw_modelmat", "overlay_edit_mesh_common");

GPU_SHADER_CREATE_INFO(overlay_edit_mesh_vert_next)
    .do_static_compilation(true)
    .builtins(BuiltinBits::POINT_SIZE)
    .define("VERT")
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_in(1, Type::UVEC4, "data")
    .vertex_in(2, Type::VEC3, "vnor")
    .vertex_source("overlay_edit_mesh_vertex_vert.glsl")
    .vertex_out(overlay_edit_mesh_vert_iface)
    .fragment_source("overlay_point_varying_color_frag.glsl")
    .additional_info("draw_view",
                     "draw_modelmat_new",
                     "draw_resource_handle_new",
                     "overlay_edit_mesh_common");

GPU_SHADER_INTERFACE_INFO(overlay_edit_mesh_edge_iface, "geometry_in")
    .smooth(Type::VEC4, "finalColor_")
    .smooth(Type::VEC4, "finalColorOuter_")
    .smooth(Type::UINT, "selectOverride_");

GPU_SHADER_INTERFACE_INFO(overlay_edit_mesh_edge_geom_iface, "geometry_out")
    .smooth(Type::VEC4, "finalColor");
GPU_SHADER_INTERFACE_INFO(overlay_edit_mesh_edge_geom_flat_iface, "geometry_flat_out")
    .flat(Type::VEC4, "finalColorOuter");
GPU_SHADER_INTERFACE_INFO(overlay_edit_mesh_edge_geom_noperspective_iface,
                          "geometry_noperspective_out")
    .no_perspective(Type::FLOAT, "edgeCoord");

GPU_SHADER_CREATE_INFO(overlay_edit_mesh_edge)
    .do_static_compilation(true)
    .define("EDGE")
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_in(1, Type::UVEC4, "data")
    .vertex_in(2, Type::VEC3, "vnor")
    .push_constant(Type::BOOL, "do_smooth_wire")
    .vertex_source("overlay_edit_mesh_vert.glsl")
    .vertex_out(overlay_edit_mesh_edge_iface)
    .geometry_out(overlay_edit_mesh_edge_geom_iface)
    .geometry_out(overlay_edit_mesh_edge_geom_flat_iface)
    .geometry_out(overlay_edit_mesh_edge_geom_noperspective_iface)
    .geometry_layout(PrimitiveIn::LINES, PrimitiveOut::TRIANGLE_STRIP, 4)
    .geometry_source("overlay_edit_mesh_geom.glsl")
    .fragment_source("overlay_edit_mesh_frag.glsl")
    .additional_info("draw_modelmat", "overlay_edit_mesh_common");

/* The Non-Geometry shader variant passes directly to fragment. */
#ifdef WITH_METAL_BACKEND
GPU_SHADER_CREATE_INFO(overlay_edit_mesh_edge_no_geom)
    .metal_backend_only(true)
    .do_static_compilation(true)
    .define("EDGE")
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_in(1, Type::UCHAR4, "data")
    .vertex_in(2, Type::VEC3_101010I2, "vnor")
    .push_constant(Type::BOOL, "do_smooth_wire")
    .vertex_out(overlay_edit_mesh_edge_geom_iface)
    .vertex_out(overlay_edit_mesh_edge_geom_flat_iface)
    .vertex_out(overlay_edit_mesh_edge_geom_noperspective_iface)
    .fragment_source("overlay_edit_mesh_frag.glsl")
    .additional_info("draw_modelmat", "overlay_edit_mesh_common_no_geom");
#endif

/* Vertex Pull version for overlay next. */
GPU_SHADER_CREATE_INFO(overlay_edit_mesh_edge_next)
    .do_static_compilation(true)
    .define("EDGE")
    .storage_buf(0, Qualifier::READ, "float", "pos[]", Frequency::GEOMETRY)
    .storage_buf(1, Qualifier::READ, "uint", "vnor[]", Frequency::GEOMETRY)
    .storage_buf(2, Qualifier::READ, "uint", "data[]", Frequency::GEOMETRY)
    .push_constant(Type::IVEC2, "gpu_attr_0")
    .push_constant(Type::IVEC2, "gpu_attr_1")
    .push_constant(Type::IVEC2, "gpu_attr_2")
    .push_constant(Type::BOOL, "do_smooth_wire")
    .push_constant(Type::BOOL, "use_vertex_selection")
    .vertex_out(overlay_edit_mesh_edge_geom_iface)
    .vertex_out(overlay_edit_mesh_edge_geom_flat_iface)
    .vertex_out(overlay_edit_mesh_edge_geom_noperspective_iface)
    .vertex_source("overlay_edit_mesh_edge_vert.glsl")
    .fragment_source("overlay_edit_mesh_frag.glsl")
    .additional_info("draw_view",
                     "draw_modelmat_new",
                     "draw_resource_handle_new",
                     "gpu_index_load",
                     "overlay_edit_mesh_common");

GPU_SHADER_CREATE_INFO(overlay_edit_mesh_edge_flat)
    .do_static_compilation(true)
    .define("FLAT")
    .additional_info("overlay_edit_mesh_edge");

#ifdef WITH_METAL_BACKEND
GPU_SHADER_CREATE_INFO(overlay_edit_mesh_edge_flat_no_geom)
    .metal_backend_only(true)
    .do_static_compilation(true)
    .define("FLAT")
    .additional_info("overlay_edit_mesh_edge_no_geom");
#endif

GPU_SHADER_CREATE_INFO(overlay_edit_mesh_face)
    .do_static_compilation(true)
    .define("FACE")
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_in(1, Type::UVEC4, "data")
    .vertex_source("overlay_edit_mesh_vert.glsl")
    .vertex_out(overlay_edit_flat_color_iface)
    .fragment_source("overlay_varying_color.glsl")
    .additional_info("draw_modelmat", "overlay_edit_mesh_common");

GPU_SHADER_CREATE_INFO(overlay_edit_mesh_face_next)
    .do_static_compilation(true)
    .define("FACE")
    .define("vnor", "vec3(0.0)")
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_in(1, Type::UVEC4, "data")
    .vertex_source("overlay_edit_mesh_face_vert.glsl")
    .vertex_out(overlay_edit_flat_color_iface)
    .fragment_source("overlay_varying_color.glsl")
    .additional_info("draw_view",
                     "draw_modelmat_new",
                     "draw_resource_handle_new",
                     "overlay_edit_mesh_common");

GPU_SHADER_CREATE_INFO(overlay_edit_mesh_facedot)
    .do_static_compilation(true)
    .define("FACEDOT")
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_in(1, Type::UVEC4, "data")
    .vertex_in(2, Type::VEC4, "norAndFlag")
    .define("vnor", "norAndFlag.xyz")
    .vertex_source("overlay_edit_mesh_vert.glsl")
    .vertex_out(overlay_edit_flat_color_iface)
    .fragment_source("overlay_point_varying_color_frag.glsl")
    .additional_info("draw_modelmat", "overlay_edit_mesh_common");

GPU_SHADER_CREATE_INFO(overlay_edit_mesh_facedot_next)
    .do_static_compilation(true)
    .define("FACEDOT")
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_in(1, Type::UVEC4, "data")
    .vertex_in(2, Type::VEC4, "norAndFlag")
    .define("vnor", "norAndFlag.xyz")
    .vertex_source("overlay_edit_mesh_facedot_vert.glsl")
    .vertex_out(overlay_edit_flat_color_iface)
    .fragment_source("overlay_point_varying_color_frag.glsl")
    .additional_info("draw_view",
                     "draw_modelmat_new",
                     "draw_resource_handle_new",
                     "overlay_edit_mesh_common");

GPU_SHADER_CREATE_INFO(overlay_edit_mesh_normal)
    .do_static_compilation(true)
    .define("WORKAROUND_INDEX_LOAD_INCLUDE")
    /* WORKAROUND: Needed to support OpenSubdiv vertex format. Should be removed. */
    .push_constant(Type::IVEC2, "gpu_attr_0")
    .push_constant(Type::IVEC2, "gpu_attr_1")
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_in(1, Type::VEC4, "lnor")
    .vertex_in(2, Type::VEC4, "vnor")
    .vertex_in(3, Type::VEC4, "norAndFlag")
    .sampler(0, ImageType::DEPTH_2D, "depthTex")
    .push_constant(Type::FLOAT, "normalSize")
    .push_constant(Type::FLOAT, "normalScreenSize")
    .push_constant(Type::FLOAT, "alpha")
    .push_constant(Type::BOOL, "isConstantScreenSizeNormals")
    .vertex_out(overlay_edit_flat_color_iface)
    .fragment_out(0, Type::VEC4, "fragColor")
    .vertex_source("overlay_edit_mesh_normal_vert.glsl")
    .fragment_source("overlay_varying_color.glsl")
    .additional_info("draw_modelmat_instanced_attr", "draw_globals");

GPU_SHADER_INTERFACE_INFO(overlay_edit_mesh_analysis_iface, "").smooth(Type::VEC4, "weightColor");

GPU_SHADER_CREATE_INFO(overlay_edit_mesh_analysis)
    .do_static_compilation(true)
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_in(1, Type::FLOAT, "weight")
    .sampler(0, ImageType::FLOAT_1D, "weightTex")
    .fragment_out(0, Type::VEC4, "fragColor")
    .vertex_out(overlay_edit_mesh_analysis_iface)
    .vertex_source("overlay_edit_mesh_analysis_vert.glsl")
    .fragment_source("overlay_edit_mesh_analysis_frag.glsl")
    .additional_info("draw_modelmat");

GPU_SHADER_CREATE_INFO(overlay_edit_mesh_skin_root)
    .do_static_compilation(true)
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_in(1, Type::FLOAT, "size")
    .vertex_in(2, Type::VEC3, "local_pos")
    .vertex_out(overlay_edit_flat_color_iface)
    .fragment_out(0, Type::VEC4, "fragColor")
    .vertex_source("overlay_edit_mesh_skin_root_vert.glsl")
    .fragment_source("overlay_varying_color.glsl")
    .additional_info("draw_modelmat_instanced_attr", "draw_globals");

GPU_SHADER_CREATE_INFO(overlay_edit_mesh_vert_clipped)
    .do_static_compilation(true)
    .additional_info("overlay_edit_mesh_vert", "drw_clipped");

GPU_SHADER_CREATE_INFO(overlay_edit_mesh_edge_clipped)
    .do_static_compilation(true)
    .additional_info("overlay_edit_mesh_edge", "drw_clipped");

#ifdef WITH_METAL_BACKEND
GPU_SHADER_CREATE_INFO(overlay_edit_mesh_edge_clipped_no_geom)
    .metal_backend_only(true)
    .do_static_compilation(true)
    .additional_info("overlay_edit_mesh_edge_no_geom", "drw_clipped");
#endif

GPU_SHADER_CREATE_INFO(overlay_edit_mesh_edge_flat_clipped)
    .do_static_compilation(true)
    .additional_info("overlay_edit_mesh_edge_flat", "drw_clipped");

#ifdef WITH_METAL_BACKEND
GPU_SHADER_CREATE_INFO(overlay_edit_mesh_edge_flat_clipped_no_geom)
    .metal_backend_only(true)
    .do_static_compilation(true)
    .additional_info("overlay_edit_mesh_edge_flat_no_geom", "drw_clipped");
#endif

GPU_SHADER_CREATE_INFO(overlay_edit_mesh_face_clipped)
    .do_static_compilation(true)
    .additional_info("overlay_edit_mesh_face", "drw_clipped");

GPU_SHADER_CREATE_INFO(overlay_edit_mesh_facedot_clipped)
    .do_static_compilation(true)
    .additional_info("overlay_edit_mesh_facedot", "drw_clipped");

GPU_SHADER_CREATE_INFO(overlay_edit_mesh_normal_clipped)
    .do_static_compilation(true)
    .additional_info("overlay_edit_mesh_normal", "drw_clipped");

GPU_SHADER_CREATE_INFO(overlay_edit_mesh_analysis_clipped)
    .do_static_compilation(true)
    .additional_info("overlay_edit_mesh_analysis", "drw_clipped");

GPU_SHADER_CREATE_INFO(overlay_edit_mesh_skin_root_clipped)
    .do_static_compilation(true)
    .additional_info("overlay_edit_mesh_skin_root", "drw_clipped");

/** \} */

/* -------------------------------------------------------------------- */
/** \name Edit UV
 * \{ */

GPU_SHADER_INTERFACE_INFO(overlay_edit_uv_iface, "geom_in").smooth(Type::FLOAT, "selectionFac");
GPU_SHADER_INTERFACE_INFO(overlay_edit_uv_flat_iface, "geom_flat_in")
    .flat(Type::VEC2, "stippleStart");
GPU_SHADER_INTERFACE_INFO(overlay_edit_uv_noperspective_iface, "geom_noperspective_in")
    .no_perspective(Type::VEC2, "stipplePos");

GPU_SHADER_INTERFACE_INFO(overlay_edit_uv_geom_iface, "geom_out")
    .smooth(Type::FLOAT, "selectionFac");
GPU_SHADER_INTERFACE_INFO(overlay_edit_uv_geom_flat_iface, "geom_flat_out")
    .flat(Type::VEC2, "stippleStart");
GPU_SHADER_INTERFACE_INFO(overlay_edit_uv_geom_noperspective_iface, "geom_noperspective_out")
    .no_perspective(Type::FLOAT, "edgeCoord")
    .no_perspective(Type::VEC2, "stipplePos");

GPU_SHADER_CREATE_INFO(overlay_edit_uv_edges_common)
    .vertex_in(0, Type::VEC2, "au")
    .vertex_in(1, Type::INT, "flag")
    .push_constant(Type::INT, "lineStyle")
    .push_constant(Type::BOOL, "doSmoothWire")
    .push_constant(Type::FLOAT, "alpha")
    .push_constant(Type::FLOAT, "dashLength")
    .fragment_out(0, Type::VEC4, "fragColor")
    .fragment_source("overlay_edit_uv_edges_frag.glsl")
    .additional_info("draw_mesh", "draw_globals");

GPU_SHADER_CREATE_INFO(overlay_edit_uv_edges)
    .additional_info("overlay_edit_uv_edges_common")
    .do_static_compilation(true)
    .vertex_out(overlay_edit_uv_iface)
    .vertex_out(overlay_edit_uv_flat_iface)
    .vertex_out(overlay_edit_uv_noperspective_iface)
    .geometry_layout(PrimitiveIn::LINES, PrimitiveOut::TRIANGLE_STRIP, 4)
    .geometry_out(overlay_edit_uv_geom_iface)
    .geometry_out(overlay_edit_uv_geom_flat_iface)
    .geometry_out(overlay_edit_uv_geom_noperspective_iface)
    .vertex_source("overlay_edit_uv_edges_vert.glsl")
    .geometry_source("overlay_edit_uv_edges_geom.glsl");

#ifdef WITH_METAL_BACKEND
GPU_SHADER_CREATE_INFO(overlay_edit_uv_edges_no_geom)
    .metal_backend_only(true)
    .additional_info("overlay_edit_uv_edges_common")
    .do_static_compilation(true)
    .vertex_out(overlay_edit_uv_geom_iface)
    .vertex_out(overlay_edit_uv_geom_flat_iface)
    .vertex_out(overlay_edit_uv_geom_noperspective_iface)
    .vertex_source("overlay_edit_uv_edges_vert_no_geom.glsl");
#endif

GPU_SHADER_CREATE_INFO(overlay_edit_uv_edges_select)
    .do_static_compilation(true)
    .define("USE_EDGE_SELECT")
    .additional_info("overlay_edit_uv_edges");

GPU_SHADER_INTERFACE_INFO(overlay_edit_uv_next_iface, "")
    .smooth(Type::FLOAT, "selectionFac")
    .flat(Type::VEC2, "stippleStart")
    .no_perspective(Type::FLOAT, "edgeCoord")
    .no_perspective(Type::VEC2, "stipplePos");

GPU_SHADER_CREATE_INFO(overlay_edit_uv_edges_next)
    .do_static_compilation(true)
    .storage_buf(0, Qualifier::READ, "float", "au[]", Frequency::GEOMETRY)
    .storage_buf(1, Qualifier::READ, "uint", "data[]", Frequency::GEOMETRY)
    .push_constant(Type::IVEC2, "gpu_attr_0")
    .push_constant(Type::IVEC2, "gpu_attr_1")
    .push_constant(Type::INT, "lineStyle")
    .push_constant(Type::BOOL, "doSmoothWire")
    .push_constant(Type::FLOAT, "alpha")
    .push_constant(Type::FLOAT, "dashLength")
    .specialization_constant(Type::BOOL, "use_edge_select", false)
    .vertex_out(overlay_edit_uv_next_iface)
    .fragment_out(0, Type::VEC4, "fragColor")
    .vertex_source("overlay_edit_uv_edges_next_vert.glsl")
    .fragment_source("overlay_edit_uv_edges_next_frag.glsl")
    .additional_info("draw_view",
                     "draw_modelmat_new",
                     "draw_resource_handle_new",
                     "gpu_index_load",
                     "draw_globals");

GPU_SHADER_CREATE_INFO(overlay_edit_uv_faces)
    .do_static_compilation(true)
    .vertex_in(0, Type::VEC2, "au")
    .vertex_in(1, Type::UINT, "flag")
    .push_constant(Type::FLOAT, "uvOpacity")
    .vertex_out(overlay_edit_flat_color_iface)
    .fragment_out(0, Type::VEC4, "fragColor")
    .vertex_source("overlay_edit_uv_faces_vert.glsl")
    .fragment_source("overlay_varying_color.glsl")
    .additional_info("draw_mesh", "draw_globals");

GPU_SHADER_CREATE_INFO(overlay_edit_uv_face_dots)
    .do_static_compilation(true)
    .vertex_in(0, Type::VEC2, "au")
    .vertex_in(1, Type::UINT, "flag")
    .push_constant(Type::FLOAT, "pointSize")
    .vertex_out(overlay_edit_flat_color_iface)
    .fragment_out(0, Type::VEC4, "fragColor")
    .vertex_source("overlay_edit_uv_face_dots_vert.glsl")
    .fragment_source("overlay_varying_color.glsl")
    .additional_info("draw_mesh", "draw_globals");

GPU_SHADER_INTERFACE_INFO(overlay_edit_uv_vert_iface, "")
    .smooth(Type::VEC4, "fillColor")
    .smooth(Type::VEC4, "outlineColor")
    .smooth(Type::VEC4, "radii");

GPU_SHADER_CREATE_INFO(overlay_edit_uv_verts)
    .do_static_compilation(true)
    .vertex_in(0, Type::VEC2, "au")
    .vertex_in(1, Type::UINT, "flag")
    .push_constant(Type::FLOAT, "pointSize")
    .push_constant(Type::FLOAT, "outlineWidth")
    .push_constant(Type::VEC4, "color")
    .vertex_out(overlay_edit_uv_vert_iface)
    .fragment_out(0, Type::VEC4, "fragColor")
    .vertex_source("overlay_edit_uv_verts_vert.glsl")
    .fragment_source("overlay_edit_uv_verts_frag.glsl")
    .additional_info("draw_mesh", "draw_globals");

GPU_SHADER_CREATE_INFO(overlay_edit_uv_tiled_image_borders)
    .do_static_compilation(true)
    .vertex_in(0, Type::VEC3, "pos")
    .push_constant(Type::VEC4, "ucolor")
    .fragment_out(0, Type::VEC4, "fragColor")
    .vertex_source("overlay_edit_uv_tiled_image_borders_vert.glsl")
    .fragment_source("overlay_uniform_color_frag.glsl")
    .additional_info("draw_mesh");

GPU_SHADER_INTERFACE_INFO(edit_uv_image_iface, "").smooth(Type::VEC2, "uvs");

GPU_SHADER_CREATE_INFO(overlay_edit_uv_stencil_image)
    .do_static_compilation(true)
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_out(edit_uv_image_iface)
    .vertex_source("overlay_edit_uv_image_vert.glsl")
    .sampler(0, ImageType::FLOAT_2D, "imgTexture")
    .push_constant(Type::BOOL, "imgPremultiplied")
    .push_constant(Type::BOOL, "imgAlphaBlend")
    .push_constant(Type::VEC4, "ucolor")
    .fragment_out(0, Type::VEC4, "fragColor")
    .fragment_source("overlay_image_frag.glsl")
    .additional_info("draw_mesh");

GPU_SHADER_CREATE_INFO(overlay_edit_uv_mask_image)
    .do_static_compilation(true)
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_out(edit_uv_image_iface)
    .sampler(0, ImageType::FLOAT_2D, "imgTexture")
    .push_constant(Type::VEC4, "color")
    .push_constant(Type::FLOAT, "opacity")
    .fragment_out(0, Type::VEC4, "fragColor")
    .vertex_source("overlay_edit_uv_image_vert.glsl")
    .fragment_source("overlay_edit_uv_image_mask_frag.glsl")
    .additional_info("draw_mesh");

/** \} */

/* -------------------------------------------------------------------- */
/** \name UV Stretching
 * \{ */

GPU_SHADER_CREATE_INFO(overlay_edit_uv_stretching)
    .vertex_in(0, Type::VEC2, "pos")
    .push_constant(Type::VEC2, "aspect")
    .push_constant(Type::FLOAT, "stretch_opacity")
    .vertex_out(overlay_edit_nopersp_color_iface)
    .fragment_out(0, Type::VEC4, "fragColor")
    .vertex_source("overlay_edit_uv_stretching_vert.glsl")
    .fragment_source("overlay_varying_color.glsl")
    .push_constant(Type::FLOAT, "totalAreaRatio");

GPU_SHADER_CREATE_INFO(overlay_edit_uv_stretching_area)
    .do_static_compilation(true)
    .vertex_in(1, Type::FLOAT, "ratio")
    .push_constant(Type::FLOAT, "totalAreaRatio")
    .additional_info("overlay_edit_uv_stretching", "draw_mesh", "draw_globals");

GPU_SHADER_CREATE_INFO(overlay_edit_uv_stretching_angle)
    .do_static_compilation(true)
    .define("STRETCH_ANGLE")
    .vertex_in(1, Type::VEC2, "uv_angles")
    .vertex_in(2, Type::FLOAT, "angle")
    .additional_info("overlay_edit_uv_stretching", "draw_mesh", "draw_globals");

/** \} */

/* -------------------------------------------------------------------- */
/** \name Edit Curve
 * \{ */

GPU_SHADER_INTERFACE_INFO(overlay_edit_curve_handle_iface, "vert").flat(Type::UINT, "flag");

GPU_SHADER_CREATE_INFO(overlay_edit_curve_handle)
    .do_static_compilation(true)
    .typedef_source("overlay_shader_shared.h")
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_in(1, Type::UINT, "data")
    .vertex_out(overlay_edit_curve_handle_iface)
    .geometry_layout(PrimitiveIn::LINES, PrimitiveOut::TRIANGLE_STRIP, 10)
    .geometry_out(overlay_edit_smooth_color_iface)
    .push_constant(Type::BOOL, "showCurveHandles")
    .push_constant(Type::INT, "curveHandleDisplay")
    .fragment_out(0, Type::VEC4, "fragColor")
    .vertex_source("overlay_edit_curve_handle_vert.glsl")
    .geometry_source("overlay_edit_curve_handle_geom.glsl")
    .fragment_source("overlay_varying_color.glsl")
    .additional_info("draw_mesh", "draw_globals");

#ifdef WITH_METAL_BACKEND
GPU_SHADER_CREATE_INFO(overlay_edit_curve_handle_no_geom)
    .metal_backend_only(true)
    .do_static_compilation(true)
    .typedef_source("overlay_shader_shared.h")
    /* NOTE: Color already in Linear space. Which is what we want. */
    .define("srgbTarget", "false")
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_in(1, Type::UINT, "data")
    .vertex_out(overlay_edit_smooth_color_iface)
    .push_constant(Type::BOOL, "showCurveHandles")
    .push_constant(Type::INT, "curveHandleDisplay")
    .fragment_out(0, Type::VEC4, "fragColor")
    .vertex_source("overlay_edit_curve_handle_vert_no_geom.glsl")
    .fragment_source("overlay_varying_color.glsl")
    .additional_info("draw_mesh", "draw_globals");
#endif

GPU_SHADER_CREATE_INFO(overlay_edit_curve_handle_next)
    .do_static_compilation(true)
    .typedef_source("overlay_shader_shared.h")
    .storage_buf(0, Qualifier::READ, "float", "pos[]", Frequency::GEOMETRY)
    .storage_buf(1, Qualifier::READ, "uint", "data[]", Frequency::GEOMETRY)
    .push_constant(Type::IVEC2, "gpu_attr_0")
    .push_constant(Type::IVEC2, "gpu_attr_1")
    .vertex_out(overlay_edit_smooth_color_iface)
    .push_constant(Type::BOOL, "showCurveHandles")
    .push_constant(Type::INT, "curveHandleDisplay")
    .push_constant(Type::FLOAT, "alpha")
    .fragment_out(0, Type::VEC4, "fragColor")
    .vertex_source("overlay_edit_curve_handle_next_vert.glsl")
    .fragment_source("overlay_varying_color.glsl")
    .additional_info("draw_view",
                     "draw_modelmat_new",
                     "draw_resource_handle_new",
                     "gpu_index_load",
                     "draw_globals");

GPU_SHADER_CREATE_INFO(overlay_edit_curves_handle_next)
    .do_static_compilation(true)
    .typedef_source("overlay_shader_shared.h")
    .storage_buf(0, Qualifier::READ, "float", "pos[]", Frequency::GEOMETRY)
    .storage_buf(1, Qualifier::READ, "uint", "data[]", Frequency::GEOMETRY)
    .storage_buf(2, Qualifier::READ, "float", "selection[]", Frequency::GEOMETRY)
    .push_constant(Type::IVEC2, "gpu_attr_0")
    .push_constant(Type::IVEC2, "gpu_attr_1")
    .push_constant(Type::IVEC2, "gpu_attr_2")
    .vertex_out(overlay_edit_smooth_color_iface)
    .push_constant(Type::BOOL, "showCurveHandles")
    .push_constant(Type::INT, "curveHandleDisplay")
    .fragment_out(0, Type::VEC4, "fragColor")
    .vertex_source("overlay_edit_curves_handle_next_vert.glsl")
    .fragment_source("overlay_varying_color.glsl")
    .additional_info("draw_view",
                     "draw_modelmat_new",
                     "draw_resource_handle_new",
                     "gpu_index_load",
                     "draw_globals");

GPU_SHADER_CREATE_INFO(overlay_edit_curve_handle_clipped)
    .do_static_compilation(true)
    .additional_info("overlay_edit_curve_handle", "drw_clipped");

#ifdef WITH_METAL_BACKEND
GPU_SHADER_CREATE_INFO(overlay_edit_curve_handle_clipped_no_geom)
    .metal_backend_only(true)
    .do_static_compilation(true)
    .additional_info("overlay_edit_curve_handle_no_geom", "drw_clipped");
#endif

GPU_SHADER_CREATE_INFO(overlay_edit_curve_point)
    .do_static_compilation(true)
    .typedef_source("overlay_shader_shared.h")
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_in(1, Type::UINT, "data")
    .vertex_out(overlay_edit_flat_color_iface)
    .push_constant(Type::BOOL, "showCurveHandles")
    .push_constant(Type::INT, "curveHandleDisplay")
    .fragment_out(0, Type::VEC4, "fragColor")
    .vertex_source("overlay_edit_curve_point_vert.glsl")
    .fragment_source("overlay_point_varying_color_frag.glsl")
    .additional_info("draw_mesh", "draw_globals");

GPU_SHADER_CREATE_INFO(overlay_edit_curve_point_clipped)
    .do_static_compilation(true)
    .additional_info("overlay_edit_curve_point", "drw_clipped");

GPU_SHADER_CREATE_INFO(overlay_edit_curve_wire)
    .do_static_compilation(true)
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_in(1, Type::VEC3, "nor")
    .vertex_in(2, Type::VEC3, "tan")
    .vertex_in(3, Type::FLOAT, "rad")
    .push_constant(Type::FLOAT, "normalSize")
    .vertex_out(overlay_edit_flat_color_iface)
    .fragment_out(0, Type::VEC4, "fragColor")
    .vertex_source("overlay_edit_curve_wire_vert.glsl")
    .fragment_source("overlay_varying_color.glsl")
    .additional_info("draw_modelmat", "draw_resource_id_uniform", "draw_globals");

GPU_SHADER_CREATE_INFO(overlay_edit_curve_wire_clipped)
    .do_static_compilation(true)
    .additional_info("overlay_edit_curve_wire", "drw_clipped");

GPU_SHADER_CREATE_INFO(overlay_edit_curve_normals)
    .do_static_compilation(true)
    .storage_buf(0, Qualifier::READ, "float", "pos[]", Frequency::GEOMETRY)
    .storage_buf(1, Qualifier::READ, "float", "rad[]", Frequency::GEOMETRY)
    .storage_buf(2, Qualifier::READ, "uint", "nor[]", Frequency::GEOMETRY)
    .storage_buf(3, Qualifier::READ, "uint", "tan[]", Frequency::GEOMETRY)
    .push_constant(Type::IVEC2, "gpu_attr_0")
    .push_constant(Type::IVEC2, "gpu_attr_1")
    .push_constant(Type::IVEC2, "gpu_attr_2")
    .push_constant(Type::IVEC2, "gpu_attr_3")
    .push_constant(Type::FLOAT, "normalSize")
    .push_constant(Type::BOOL, "use_hq_normals")
    .vertex_out(overlay_edit_flat_color_iface)
    .fragment_out(0, Type::VEC4, "fragColor")
    .vertex_source("overlay_edit_curve_wire_next_vert.glsl")
    .fragment_source("overlay_varying_color.glsl")
    .additional_info("draw_view",
                     "draw_modelmat_new",
                     "draw_resource_handle_new",
                     "gpu_index_load",
                     "draw_globals");

/** \} */

/* -------------------------------------------------------------------- */
/** \name Edit Curves
 * \{ */

GPU_SHADER_INTERFACE_INFO(overlay_edit_curves_handle_iface, "vert")
    .flat(Type::UINT, "flag")
    .flat(Type::FLOAT, "selection");

GPU_SHADER_CREATE_INFO(overlay_edit_curves_handle)
    .do_static_compilation(true)
    .typedef_source("overlay_shader_shared.h")
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_in(1, Type::UINT, "data")
    .vertex_in(2, Type::FLOAT, "selection")
    .vertex_out(overlay_edit_curves_handle_iface)
    .geometry_layout(PrimitiveIn::LINES, PrimitiveOut::TRIANGLE_STRIP, 10)
    .geometry_out(overlay_edit_smooth_color_iface)
    .fragment_out(0, Type::VEC4, "fragColor")
    .vertex_source("overlay_edit_curves_handle_vert.glsl")
    .geometry_source("overlay_edit_curves_handle_geom.glsl")
    .fragment_source("overlay_varying_color.glsl")
    .additional_info("draw_mesh", "draw_globals");

GPU_SHADER_CREATE_INFO(overlay_edit_curves_handle_clipped)
    .do_static_compilation(true)
    .additional_info("overlay_edit_curves_handle", "drw_clipped");

#ifdef WITH_METAL_BACKEND
GPU_SHADER_CREATE_INFO(overlay_edit_curves_handle_no_geom)
    .metal_backend_only(true)
    .do_static_compilation(true)
    .typedef_source("overlay_shader_shared.h")
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_in(1, Type::UINT, "data")
    .vertex_in(2, Type::FLOAT, "selection")
    .vertex_out(overlay_edit_smooth_color_iface)
    .fragment_out(0, Type::VEC4, "fragColor")
    .vertex_source("overlay_edit_curves_handle_vert_no_geom.glsl")
    .fragment_source("overlay_varying_color.glsl")
    .additional_info("draw_mesh", "draw_globals");

GPU_SHADER_CREATE_INFO(overlay_edit_curves_handle_clipped_no_geom)
    .metal_backend_only(true)
    .do_static_compilation(true)
    .additional_info("overlay_edit_curves_handle_no_geom", "drw_clipped");
#endif

/** \} */

/* -------------------------------------------------------------------- */
/** \name Edit Lattice
 * \{ */

GPU_SHADER_CREATE_INFO(overlay_edit_lattice_point)
    .do_static_compilation(true)
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_in(1, Type::UINT, "data")
    .vertex_out(overlay_edit_flat_color_iface)
    .fragment_out(0, Type::VEC4, "fragColor")
    .vertex_source("overlay_edit_lattice_point_vert.glsl")
    .fragment_source("overlay_point_varying_color_frag.glsl")
    .additional_info("draw_mesh", "draw_globals");

GPU_SHADER_CREATE_INFO(overlay_edit_lattice_point_clipped)
    .do_static_compilation(true)
    .additional_info("overlay_edit_lattice_point", "drw_clipped");

GPU_SHADER_CREATE_INFO(overlay_edit_lattice_wire)
    .do_static_compilation(true)
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_in(1, Type::FLOAT, "weight")
    .sampler(0, ImageType::FLOAT_1D, "weightTex")
    .vertex_out(overlay_edit_smooth_color_iface)
    .fragment_out(0, Type::VEC4, "fragColor")
    .vertex_source("overlay_edit_lattice_wire_vert.glsl")
    .fragment_source("overlay_varying_color.glsl")
    .additional_info("draw_mesh", "draw_globals");

GPU_SHADER_CREATE_INFO(overlay_edit_lattice_wire_clipped)
    .do_static_compilation(true)
    .additional_info("overlay_edit_lattice_wire", "drw_clipped");

/** \} */

/* -------------------------------------------------------------------- */
/** \name Edit Particle
 * \{ */

GPU_SHADER_CREATE_INFO(overlay_edit_particle_strand)
    .do_static_compilation(true)
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_in(1, Type::FLOAT, "selection")
    .sampler(0, ImageType::FLOAT_1D, "weightTex")
    .push_constant(Type::BOOL, "useWeight")
    .push_constant(Type::BOOL, "useGreasePencil")
    .vertex_out(overlay_edit_smooth_color_iface)
    .fragment_out(0, Type::VEC4, "fragColor")
    .vertex_source("overlay_edit_particle_strand_vert.glsl")
    .fragment_source("overlay_varying_color.glsl")
    .additional_info("draw_mesh", "draw_globals");

GPU_SHADER_CREATE_INFO(overlay_edit_particle_strand_clipped)
    .do_static_compilation(true)
    .additional_info("overlay_edit_particle_strand", "drw_clipped");

GPU_SHADER_CREATE_INFO(overlay_edit_particle_point)
    .do_static_compilation(true)
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_in(1, Type::FLOAT, "selection")
    .vertex_out(overlay_edit_flat_color_iface)
    .sampler(0, ImageType::FLOAT_1D, "weightTex")
    .push_constant(Type::BOOL, "useWeight")
    .push_constant(Type::BOOL, "useGreasePencil")
    .fragment_out(0, Type::VEC4, "fragColor")
#if 1 /* TODO(fclem): Required for legacy gpencil overlay. To be moved to specialized shader. */
    .typedef_source("overlay_shader_shared.h")
    .vertex_in(3, Type::UINT, "vflag")
    .push_constant(Type::BOOL, "doStrokeEndpoints")
#endif
    .vertex_source("overlay_edit_particle_point_vert.glsl")
    .fragment_source("overlay_point_varying_color_frag.glsl")
    .additional_info("draw_mesh", "draw_globals");

GPU_SHADER_CREATE_INFO(overlay_edit_particle_point_clipped)
    .do_static_compilation(true)
    .additional_info("overlay_edit_particle_point", "drw_clipped");

/** \} */

/* -------------------------------------------------------------------- */
/** \name Edit GPencil
 * \{ */

GPU_SHADER_CREATE_INFO(overlay_edit_gpencil)
    .typedef_source("overlay_shader_shared.h")
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_in(1, Type::INT, "ma")
    .vertex_in(2, Type::UINT, "vflag")
    .vertex_in(3, Type::FLOAT, "weight")
    .push_constant(Type::FLOAT, "normalSize")
    .push_constant(Type::BOOL, "doMultiframe")
    .push_constant(Type::BOOL, "doStrokeEndpoints")
    .push_constant(Type::BOOL, "hideSelect")
    .push_constant(Type::BOOL, "doWeightColor")
    .push_constant(Type::FLOAT, "gpEditOpacity")
    .push_constant(Type::VEC4, "gpEditColor")
    .sampler(0, ImageType::FLOAT_1D, "weightTex")
    .fragment_out(0, Type::VEC4, "fragColor")
    .vertex_source("overlay_edit_gpencil_vert.glsl")
    .additional_info("draw_mesh", "draw_globals");

GPU_SHADER_CREATE_INFO(overlay_edit_gpencil_wire)
    .do_static_compilation(true)
    .vertex_out(overlay_edit_smooth_color_iface)
    .fragment_source("overlay_varying_color.glsl")
    .additional_info("overlay_edit_gpencil");

GPU_SHADER_CREATE_INFO(overlay_edit_gpencil_wire_clipped)
    .do_static_compilation(true)
    .additional_info("overlay_edit_gpencil_wire", "drw_clipped");

GPU_SHADER_CREATE_INFO(overlay_edit_gpencil_point)
    .do_static_compilation(true)
    .define("USE_POINTS")
    .vertex_out(overlay_edit_flat_color_iface)
    .fragment_source("overlay_point_varying_color_frag.glsl")
    .additional_info("overlay_edit_gpencil");

GPU_SHADER_CREATE_INFO(overlay_edit_gpencil_point_clipped)
    .do_static_compilation(true)
    .additional_info("overlay_edit_gpencil_point", "drw_clipped");

/* TODO(fclem): Refactor this to take list of point instead of drawing 1 point per drawcall. */
GPU_SHADER_CREATE_INFO(overlay_edit_gpencil_guide_point)
    .do_static_compilation(true)
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_in(1, Type::UINT, "data")
    .vertex_out(overlay_edit_flat_color_iface)
    .push_constant(Type::VEC3, "pPosition")
    .push_constant(Type::FLOAT, "pSize")
    .push_constant(Type::VEC4, "pColor")
    .fragment_out(0, Type::VEC4, "fragColor")
    .vertex_source("overlay_edit_gpencil_guide_vert.glsl")
    .fragment_source("overlay_point_varying_color_frag.glsl")
    .additional_info("draw_mesh", "draw_globals");

GPU_SHADER_CREATE_INFO(overlay_edit_gpencil_guide_point_clipped)
    .do_static_compilation(true)
    .additional_info("overlay_edit_gpencil_guide_point", "drw_clipped");

/** \} */

/* -------------------------------------------------------------------- */
/** \name Depth Only Shader
 *
 * Used to occlude edit geometry which might not be rendered by the render engine.
 * \{ */

GPU_SHADER_CREATE_INFO(overlay_depth_only)
    .do_static_compilation(true)
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_source("overlay_depth_only_vert.glsl")
    .fragment_source("overlay_depth_only_frag.glsl")
    .additional_info("draw_mesh");

GPU_SHADER_CREATE_INFO(overlay_depth_only_clipped)
    .do_static_compilation(true)
    .additional_info("overlay_depth_only", "drw_clipped");

GPU_SHADER_CREATE_INFO(overlay_depth_mesh)
    .do_static_compilation(true)
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_source("basic_depth_vert.glsl")
    .fragment_source("overlay_depth_only_frag.glsl")
    .additional_info("draw_globals", "draw_view", "draw_modelmat_new", "draw_resource_handle_new");

GPU_SHADER_CREATE_INFO(overlay_depth_mesh_conservative)
    .do_static_compilation(true)
    .storage_buf(0, Qualifier::READ, "float", "pos[]", Frequency::GEOMETRY)
    .push_constant(Type::IVEC2, "gpu_attr_0")
    .vertex_source("overlay_depth_only_mesh_conservative_vert.glsl")
    .fragment_source("overlay_depth_only_frag.glsl")
    .additional_info("draw_globals",
                     "draw_view",
                     "draw_modelmat_new",
                     "gpu_index_load",
                     "draw_resource_handle_new");

GPU_SHADER_INTERFACE_INFO(overlay_depth_only_gpencil_flat_iface, "gp_interp_flat")
    .flat(Type::VEC2, "aspect")
    .flat(Type::VEC4, "sspos");
GPU_SHADER_INTERFACE_INFO(overlay_depth_only_gpencil_noperspective_iface,
                          "gp_interp_noperspective")
    .no_perspective(Type::VEC2, "thickness")
    .no_perspective(Type::FLOAT, "hardness");

GPU_SHADER_CREATE_INFO(overlay_depth_gpencil)
    .do_static_compilation(true)
    .typedef_source("gpencil_shader_shared.h")
    .vertex_out(overlay_depth_only_gpencil_flat_iface)
    .vertex_out(overlay_depth_only_gpencil_noperspective_iface)
    .vertex_source("overlay_depth_only_gpencil_vert.glsl")
    .fragment_source("overlay_depth_only_gpencil_frag.glsl")
    .depth_write(DepthWrite::ANY)
    .push_constant(Type::BOOL, "gpStrokeOrder3d") /* TODO(fclem): Move to a GPencil object UBO. */
    .push_constant(Type::VEC4, "gpDepthPlane")    /* TODO(fclem): Move to a GPencil object UBO. */
    .additional_info("draw_view",
                     "draw_modelmat_new",
                     "draw_resource_handle_new",
                     "draw_globals",
                     "draw_gpencil_new",
                     "draw_object_infos_new");

GPU_SHADER_CREATE_INFO(overlay_depth_pointcloud)
    .do_static_compilation(true)
    .vertex_source("basic_depth_pointcloud_vert.glsl")
    .fragment_source("overlay_depth_only_frag.glsl")
    .additional_info("draw_pointcloud_new",
                     "draw_globals",
                     "draw_view",
                     "draw_modelmat_new",
                     "draw_resource_handle_new");

GPU_SHADER_CREATE_INFO(overlay_depth_curves)
    .do_static_compilation(true)
    .vertex_source("basic_depth_curves_vert.glsl")
    .fragment_source("overlay_depth_only_frag.glsl")
    .additional_info("draw_hair_new",
                     "draw_globals",
                     "draw_view",
                     "draw_modelmat_new",
                     "draw_resource_handle_new");

/** \} */

/* -------------------------------------------------------------------- */
/** \name Uniform color
 * \{ */

GPU_SHADER_CREATE_INFO(overlay_uniform_color)
    .do_static_compilation(true)
    .vertex_in(0, Type::VEC3, "pos")
    .push_constant(Type::VEC4, "ucolor")
    .fragment_out(0, Type::VEC4, "fragColor")
    .vertex_source("overlay_depth_only_vert.glsl")
    .fragment_source("overlay_uniform_color_frag.glsl")
    .additional_info("draw_mesh");

GPU_SHADER_CREATE_INFO(overlay_uniform_color_pointcloud)
    .do_static_compilation(true)
    .push_constant(Type::VEC4, "ucolor")
    .fragment_out(0, Type::VEC4, "fragColor")
    .vertex_source("overlay_pointcloud_only_vert.glsl")
    .fragment_source("overlay_uniform_color_frag.glsl")
    .additional_info("draw_pointcloud");

GPU_SHADER_CREATE_INFO(overlay_uniform_color_clipped)
    .do_static_compilation(true)
    .additional_info("overlay_uniform_color", "drw_clipped");

GPU_SHADER_CREATE_INFO(overlay_uniform_color_pointcloud_clipped)
    .do_static_compilation(true)
    .additional_info("overlay_uniform_color_pointcloud", "drw_clipped");

/** \} */
