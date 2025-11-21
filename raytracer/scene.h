#ifndef SCENE_H
#define SCENE_H

#include "math.h"
#include "geometry.h"
#include <vector>
#include <limits>

struct Material {
  Vector3df ambient;
  Vector3df diffuse;
  Vector3df specular;
  float reflectivity; // 0..1
};

struct Light {
  Vector3df position;
  Vector3df color; // intensity per channel
};

struct SceneObject {
  enum class Type { Sphere, Triangle } type;
  Material material;
  Sphere3df sphere;      // valid if type==Sphere
  Triangle3df triangle;  // valid if type==Triangle

  SceneObject(const Sphere3df &s, const Material &m)
      : type(Type::Sphere), material(m), sphere(s), triangle({{0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}}) {}

  SceneObject(const Triangle3df &t, const Material &m)
      : type(Type::Triangle), material(m), sphere({{0.0f,0.0f,0.0f}, 1.0f}), triangle(t) {}
};

struct HitInfo {
  bool hit = false;
  float t = 0.0f;
  Vector3df point{ {0.0f, 0.0f, 0.0f} };
  Vector3df normal{ {0.0f, 0.0f, 0.0f} };
  const SceneObject *object = nullptr;
};

class Scene {
public:
  std::vector<SceneObject> objects;
  std::vector<Light> lights;
  Vector3df background{ {0.0f, 0.0f, 0.0f} };

  bool intersect(const Ray3df &ray, HitInfo &hit) const {
    bool any = false;
    float closest = std::numeric_limits<float>::max();

    for (const auto &obj : objects) {
      bool local_hit = false;
      Intersection_Context<float,3u> local_ctx;
      switch (obj.type) {
        case SceneObject::Type::Sphere:
          local_hit = obj.sphere.intersects(ray, local_ctx);
          break;
        case SceneObject::Type::Triangle:
          local_hit = obj.triangle.intersects(ray, local_ctx);
          break;
      }
      if (local_hit && local_ctx.t > 0.0f && local_ctx.t < closest) {
        closest = local_ctx.t;
        any = true;
        hit.hit = true;
        hit.t = local_ctx.t;
        hit.point = local_ctx.intersection;
        hit.normal = local_ctx.normal;
        hit.object = &obj;
      }
    }
    return any;
  }
};

#endif
#ifndef IMAGE_H
#define IMAGE_H

#include "math.h"
#include <vector>
#include <fstream>
#include <string>

class Image {
  int width_;
  int height_;
  std::vector<Vector3df> pixels_; // rgb in [0,1]
public:
  Image(int w, int h) : width_(w), height_(h), pixels_(w*h, {0.0f,0.0f,0.0f}) {}

  int width() const { return width_; }
  int height() const { return height_; }

  void set_pixel(int x, int y, const Vector3df &color) {
    if (x < 0 || x >= width_ || y < 0 || y >= height_) return;
    pixels_[y*width_ + x] = color;
  }

  const Vector3df &get_pixel(int x, int y) const {
    return pixels_[y*width_ + x];
  }

  void save_ppm(const std::string &filename) const {
    std::ofstream out(filename, std::ios::binary);
    out << "P6\n" << width_ << " " << height_ << "\n255\n";
    for (const auto &c : pixels_) {
      auto clamp = [](float v){ return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v); };
      unsigned char r = static_cast<unsigned char>(255.0f * clamp(c.vector[0]));
      unsigned char g = static_cast<unsigned char>(255.0f * clamp(c.vector[1]));
      unsigned char b = static_cast<unsigned char>(255.0f * clamp(c.vector[2]));
      out.put(r); out.put(g); out.put(b);
    }
  }
};

#endif
