package com.yoshi991.oboe.datatype

data class EffectDescription(
    val name: String,
    val category: String,
    val id: Int,
    val paramValues: Array<ParamDescription>
) {
    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (javaClass != other?.javaClass) return false
        other as EffectDescription
        if (id != other.id) return false
        return true
    }
    override fun hashCode(): Int = id
}
