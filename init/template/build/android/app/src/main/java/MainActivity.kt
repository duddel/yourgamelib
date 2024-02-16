package yourgame.project

import android.app.NativeActivity
import android.os.Bundle
import android.util.Log

class MainActivity : NativeActivity() {
    public override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
    }

    fun receiveCmd(cmdId: Int, data0: Int, data1: Int, data2: Int): Int {
        Log.d("yourgame_app",
                "received command " + cmdId + ", data: " + data0 + ", " + data1 + ", " + data2)
        return 0
    }
}
