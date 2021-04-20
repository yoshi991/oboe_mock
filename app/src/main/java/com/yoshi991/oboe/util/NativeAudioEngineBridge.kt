package com.yoshi991.oboe.util

import android.content.Context
import android.util.Log
import java.io.*

object NativeAudioEngineBridge {
    /**
     * Returns the list of internal storage paths.
     */
    @JvmStatic
    fun copyAssetToInternalStorage(
        context: Context,
        assetsFilePath: String
    ): String? {
        val assetManager = context.applicationContext.assets
        val internalStorageFilesDirAbsolutePath = context.filesDir.absolutePath

        var inputStream: InputStream? = null
        var outputStream: OutputStream? = null

        return try {
            val outFile = File(internalStorageFilesDirAbsolutePath, assetsFilePath)
            val result = outFile.absolutePath
            if (!outFile.exists()) {
                val file = requireNotNull(File(outFile.absolutePath).parentFile)
                if (!file.exists()) {
                    file.mkdirs()
                }
                inputStream = assetManager.open(assetsFilePath)
                outputStream = FileOutputStream(outFile)
                copyFile(inputStream, outputStream)
            }
            result
        } catch (e: IOException) {
            Log.e("NativeAudioEngineBridge", "$assetsFilePath not found.")
            return null
        } finally {
            try {
                inputStream?.close()
            } catch (e: IOException) {
                e.printStackTrace()
            }
            try {
                outputStream?.close()
            } catch (e: IOException) {
                e.printStackTrace()
            }
        }
    }

    private fun copyFile(inputStream: InputStream, out: OutputStream) {
        val buffer = ByteArray(1024)
        while (true) {
            val bytesRead = inputStream.read(buffer)
            if (bytesRead == -1) {
                break
            }
            out.write(buffer, 0, bytesRead)
        }
    }
}