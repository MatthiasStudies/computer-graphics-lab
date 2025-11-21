#include "math.h"
#include "geometry.h"
#include "image.h"
#include "camera.h"
#include "scene.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

// Die folgenden Kommentare beschreiben Datenstrukturen und Funktionen
// Die Datenstrukturen und Funktionen die weiter hinten im Text beschrieben sind,
// hängen höchstens von den vorhergehenden Datenstrukturen ab, aber nicht umgekehrt.



// Ein "Bildschirm", der das Setzen eines Pixels kapselt
// Der Bildschirm hat eine Auflösung (Breite x Höhe)
// Kann zur Ausgabe einer PPM-Datei verwendet werden oder
// mit SDL2 implementiert werden.



// Eine "Kamera", die von einem Augenpunkt aus in eine Richtung senkrecht auf ein Rechteck (das Bild) zeigt.
// Für das Rechteck muss die Auflösung oder alternativ die Pixelbreite und -höhe bekannt sein.
// Für ein Pixel mit Bildkoordinate kann ein Sehstrahl erzeugt werden.



// Für die "Farbe" benötigt man nicht unbedingt eine eigene Datenstruktur.
// Sie kann als Vector3df implementiert werden mit Farbanteil von 0 bis 1.
// Vor Setzen eines Pixels auf eine bestimmte Farbe (z.B. 8-Bit-Farbtiefe),
// kann der Farbanteil mit 255 multipliziert  und der Nachkommaanteil verworfen werden.


// Das "Material" der Objektoberfläche mit ambienten, diffusem und reflektiven Farbanteil.



// Ein "Objekt", z.B. eine Kugel oder ein Dreieck, und dem zugehörigen Material der Oberfläche.
// Im Prinzip ein Wrapper-Objekt, das mindestens Material und geometrisches Objekt zusammenfasst.
// Kugel und Dreieck finden Sie in geometry.h/tcc


// verschiedene Materialdefinition, z.B. Mattes Schwarz, Mattes Rot, Reflektierendes Weiss, ...
// im wesentlichen Variablen, die mit Konstruktoraufrufen initialisiert werden.


// Die folgenden Werte zur konkreten Objekten, Lichtquellen und Funktionen, wie Lambertian-Shading
// oder die Suche nach einem Sehstrahl für das dem Augenpunkt am nächsten liegenden Objekte,
// können auch zusammen in eine Datenstruktur für die gesammte zu
// rendernde "Szene" zusammengefasst werden.

// Die Cornelbox aufgebaut aus den Objekten
// Am besten verwendet man hier einen std::vector< ... > von Objekten.

// Punktförmige "Lichtquellen" können einfach als Vector3df implementiert werden mit weisser Farbe,
// bei farbigen Lichtquellen müssen die entsprechenden Daten in Objekt zusammengefaßt werden
// Bei mehreren Lichtquellen können diese in einen std::vector gespeichert werden.

// Sie benötigen eine Implementierung von Lambertian-Shading, z.B. als Funktion
// Benötigte Werte können als Parameter übergeben werden, oder wenn diese Funktion eine Objektmethode eines
// Szene-Objekts ist, dann kann auf die Werte teilweise direkt zugegriffen werden.
// Bei mehreren Lichtquellen muss der resultierende diffuse Farbanteil durch die Anzahl Lichtquellen geteilt werden.

// Für einen Sehstrahl aus allen Objekte, dasjenige finden, das dem Augenpunkt am nächsten liegt.
// Am besten einen Zeiger auf das Objekt zurückgeben. Wenn dieser nullptr ist, dann gibt es kein sichtbares Objekt.

// Die rekursive raytracing-Methode. Am besten ab einer bestimmten Rekursionstiefe (z.B. als Parameter übergeben) abbrechen.


static const int MAX_DEPTH = 4;

static Vector3df lambert(const Scene &scene, const HitInfo &hit) {
  const Material &mat = hit.object->material;
  Vector3df result = mat.ambient;

  for (const auto &light : scene.lights) {
    Vector3df to_light = light.position - hit.point;
    float dist2 = to_light.square_of_length();
    float dist = std::sqrt(dist2);
    to_light /= dist;

    // Shadow ray
    Ray3df shadow_ray{hit.point + 0.001f * hit.normal, to_light};
    HitInfo shadow_hit{};
    if (scene.intersect(shadow_ray, shadow_hit) && shadow_hit.t < dist) {
      continue; // in shadow for this light
    }

    float ndotl = hit.normal * to_light;
    if (ndotl > 0.0f) {
      Vector3df diff = ndotl * Vector3df{
        mat.diffuse[0] * light.color[0],
        mat.diffuse[1] * light.color[1],
        mat.diffuse[2] * light.color[2]
      };
      result += diff;
    }
  }
  return result;
}

static Vector3df trace_ray(const Scene &scene, const Ray3df &ray, int depth) {
  if (depth <= 0) {
    return scene.background;
  }

  HitInfo hit{};
  if (!scene.intersect(ray, hit)) {
    return scene.background;
  }

  Vector3df local_color = lambert(scene, hit);

  float refl = hit.object->material.reflectivity;
  if (refl > 0.0f) {
    Vector3df refl_dir = ray.direction.get_reflective(hit.normal);
    Ray3df refl_ray{hit.point + 0.001f * hit.normal, refl_dir};
    Vector3df refl_col = trace_ray(scene, refl_ray, depth - 1);
    local_color = (1.0f - refl) * local_color + refl * refl_col;
  }

  return local_color;
}

static Scene create_cornell_box_scene() {
  Scene scene;

  // Materials with small ambient components to make colors clearly visible
  Material red{{0.2f, 0.05f, 0.05f}, {0.9f, 0.1f, 0.1f}, {0.0f, 0.0f, 0.0f}, 0.0f};
  Material green{{0.05f, 0.2f, 0.05f}, {0.1f, 0.9f, 0.1f}, {0.0f, 0.0f, 0.0f}, 0.0f};
  Material white{{0.1f, 0.1f, 0.1f}, {0.9f, 0.9f, 0.9f}, {0.0f, 0.0f, 0.0f}, 0.0f};
  Material mirror{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 0.9f};

  // Box size
  float s = 1.0f;

  // Floor
  // scene.objects.push_back(
  //   SceneObject(Triangle3df({-s, -s, -s}, { s, -s, -s}, { s, -s,  s}), white)
  // );
  scene.objects.emplace_back(Triangle3df({-s, -s, -s}, { s, -s, -s}, { s, -s,  s}), white);
  scene.objects.emplace_back(Triangle3df({-s, -s, -s}, { s, -s,  s}, {-s, -s,  s}), white);

  // Ceiling
  scene.objects.emplace_back(Triangle3df({-s, s, -s}, { s, s,  s}, { s, s, -s}), white);
  // scene.objects.emplace_back(Triangle3df({-s, s, -s}, {-s, s,  s}, { s, s,  s}), white);

  // Back wall
  scene.objects.emplace_back(Triangle3df({-s, -s, s}, { s, -s, s}, { s,  s, s}), white);
  scene.objects.emplace_back(Triangle3df({-s, -s, s}, { s,  s, s}, {-s,  s, s}), white);

  // Left wall (red)
  scene.objects.emplace_back(Triangle3df({-s, -s, -s}, {-s, -s,  s}, {-s,  s,  s}), red);
  scene.objects.emplace_back(Triangle3df({-s, -s, -s}, {-s,  s,  s}, {-s,  s, -s}), red);

  // Right wall (green)
  scene.objects.emplace_back(Triangle3df({ s, -s, -s}, { s,  s,  s}, { s, -s,  s}), green);
  scene.objects.emplace_back(Triangle3df({ s, -s, -s}, { s,  s, -s}, { s,  s,  s}), green);

  // Mirror sphere in the middle
  scene.objects.emplace_back(Sphere3df({0.0f, -s + 0.35f, 0.0f}, 0.35f), mirror);

  // Light
  scene.lights.push_back({{0.0f, s - 0.2f, 0.0f}, {1.0f, 1.0f, 1.0f}});

  scene.background = {0.0f, 0.0f, 0.0f};

  return scene;
}

int main(void) {
  constexpr int width = 400;
  constexpr int height = 400;

  Image image(width, height);

  constexpr float aspect = static_cast<float>(width) / static_cast<float>(height);
  const Camera camera({0.0f, 0.0f, -3.5f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 45.0f, aspect);

  const Scene scene = create_cornell_box_scene();

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      const float sx = (static_cast<float>(x) + 0.5f) / static_cast<float>(width);
      const float sy = (static_cast<float>(y) + 0.5f) / static_cast<float>(height);
      Ray3df ray = camera.generate_ray(sx, sy);
      Vector3df color = trace_ray(scene, ray, MAX_DEPTH);
      image.set_pixel(x, y, color);
    }
  }

  image.save_ppm("output.ppm");

  return 0;   
}
