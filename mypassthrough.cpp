#include <gst/gst.h>
#include <gst/base/gstbasetransform.h>

/* =======================
 *  Déclarations “boilerplate”
 * ======================= */

#define GST_TYPE_MY_PASS (gst_my_pass_get_type())
G_DECLARE_FINAL_TYPE(GstMyPass, gst_my_pass,
                     GST, MY_PASS, GstBaseTransform)

struct _GstMyPass
{
  GstBaseTransform parent;
  /* Pas d’état interne pour un simple pass-through */
};

G_DEFINE_TYPE(GstMyPass, gst_my_pass, GST_TYPE_BASE_TRANSFORM)

/* =======================
 *  Fonction de traitement
 * ======================= */

/* Transform in-place : on ne touche pas au buffer */
static GstFlowReturn
gst_my_pass_transform_ip(GstBaseTransform *base, GstBuffer *buf)
{
  GST_LOG_OBJECT(base, "buffer passthrough");
  return GST_FLOW_OK;
}

/* =======================
 *  Initialisation de la classe
 * ======================= */
static void
gst_my_pass_class_init(GstMyPassClass *klass)
{

  /* -------- PAD TEMPLATES : accept “ANY” caps pour un passe-tout -------- */
  static GstStaticPadTemplate sink_templ = GST_STATIC_PAD_TEMPLATE(
      "sink",               /* nom du pad             */
      GST_PAD_SINK,         /* direction              */
      GST_PAD_ALWAYS,       /* disponibilité          */
      GST_STATIC_CAPS_ANY); /* nous acceptons tout    */

  static GstStaticPadTemplate src_templ = GST_STATIC_PAD_TEMPLATE(
      "src",
      GST_PAD_SRC,
      GST_PAD_ALWAYS,
      GST_STATIC_CAPS_ANY);

  /* Enregistrement auprès de la classe élément */
  gst_element_class_add_static_pad_template(GST_ELEMENT_CLASS(klass), &sink_templ);
  gst_element_class_add_static_pad_template(GST_ELEMENT_CLASS(klass), &src_templ);

  /* (facultatif mais pratique) : indiquer qu’on est passe-tout */
  GstBaseTransformClass *trans_class = GST_BASE_TRANSFORM_CLASS(klass);
  trans_class->passthrough_on_same_caps = TRUE;

  /* L’élément est en mode in-place, sans changement de taille */
  trans_class->transform_ip = GST_DEBUG_FUNCPTR(gst_my_pass_transform_ip);
  /* Indique qu’on accepte les caps d’entrée en sortie sans modif */
  gst_element_class_set_static_metadata(GST_ELEMENT_CLASS(klass),
                                        "Passe-tout minimal", "Filter/Effect/Video",
                                        "Ne fait rien, exemple pédagogique",
                                        "Votre Nom <vous@exemple.com>");
}

/* =======================
 *  Initialisation instance
 * ======================= */
static void
gst_my_pass_init(GstMyPass *self) { /* rien à faire */ }

/* =======================
 *  Point d’entrée du plugin
 * ======================= */
static gboolean
plugin_init(GstPlugin *plugin)
{
  /* Enregistre le type comme un nouvel élément nommé “mypassthrough” */
  return gst_element_register(plugin,
                              "mypassthrough", /* nom dans les pipelines */
                              GST_RANK_NONE,
                              GST_TYPE_MY_PASS);
}

/* =======================
 *  Déclaration du plugin
 * ======================= */
#ifndef PACKAGE
#define PACKAGE "mypassthrough"
#endif

GST_PLUGIN_DEFINE(GST_VERSION_MAJOR,
                  GST_VERSION_MINOR,
                  mypassthrough,               /* nom interne */
                  "Plugin passe-tout minimal", /* description */
                  plugin_init,
                  "1.0", /* version */
                  "LGPL",
                  "mypassthrough",
                  "https://exemple.com")
