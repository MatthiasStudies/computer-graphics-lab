#ifndef CAMERA_H
#define CAMERA_H

#include "math.h"
#include "geometry.h"

class Camera {
  Vector3df eye_;
  Vector3df forward_;
  Vector3df right_;
  Vector3df up_;
  float image_plane_dist_;
  float image_plane_width_;
  float image_plane_height_;
public:
  Camera(const Vector3df &eye,
         const Vector3df &look_at,
         const Vector3df &up,
         float fov_y_deg,
         float aspect_ratio)
      : eye_(eye),
        forward_({0.0f, 0.0f, 0.0f}),
        right_({0.0f, 0.0f, 0.0f}),
        up_({0.0f, 0.0f, 0.0f}),
        image_plane_dist_(0.0f),
        image_plane_width_(0.0f),
        image_plane_height_(0.0f) {
    forward_ = look_at - eye_;
    forward_.normalize();
    right_ = forward_.cross_product(up);
    right_.normalize();
    up_ = right_.cross_product(forward_);
    up_.normalize();

    float fov_y_rad = static_cast<float>(fov_y_deg * PI / 180.0L);
    image_plane_dist_ = 1.0f;
    image_plane_height_ = 2.0f * image_plane_dist_ * std::tan(fov_y_rad * 0.5f);
    image_plane_width_ = image_plane_height_ * aspect_ratio;
  }

  Ray3df generate_ray(float sx, float sy) const {
    Vector3df center = eye_ + image_plane_dist_ * forward_;
    Vector3df offset = (sx - 0.5f) * image_plane_width_ * right_ +
                       (sy - 0.5f) * image_plane_height_ * (-1.0f * up_);
    Vector3df dir = center + offset - eye_;
    dir.normalize();
    return Ray3df{eye_, dir};
  }
};

#endif
