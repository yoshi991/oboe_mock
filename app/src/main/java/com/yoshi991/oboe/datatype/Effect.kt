package com.yoshi991.oboe.datatype

/**
 * Class which represents an audio effect
 */
data class Effect(val effectDescription: EffectDescription) {
    val name = effectDescription.name
    val paramValues = FloatArray(effectDescription.paramValues.size) {
            i -> effectDescription.paramValues[i].defaultValue
    }
    var enable: Boolean = true
}