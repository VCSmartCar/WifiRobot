package elechen.android.wifirobot.control;

import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;

import elechen.android.wifirobot.R;
import android.content.Context;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceView;

public class MySurfaceView extends SurfaceView implements Callback {
	// 定义控件
	private SurfaceHolder sfh = null;
	private URL videoURL = null;
	private Bitmap bm = null;// 图片
	private Bitmap mBitmap = null;
	private Canvas canvas = null;// 画布
	private Paint paint = null;// 画笔
	private HttpURLConnection conn = null;
	private String CameraIP;
	private boolean isConnected = false;
	private static int mScreenWidth;
	private static int mScreenHeight;
	private InputStream inputstream = null;
	private String DEBUG_MSG = "xf_debug";
	

	public MySurfaceView(Context context, AttributeSet attrs) {
		super(context, attrs);
		initialize();
		sfh = this.getHolder();
		sfh.addCallback(this);
		paint = new Paint();// 初始化画笔
		paint.setAntiAlias(true);
		paint.setColor(Color.RED);
		this.setKeepScreenOn(true);
		setFocusable(true);
	}

	// 初始化画布大小
	private void initialize() {
		DisplayMetrics dm = getResources().getDisplayMetrics();
		mScreenWidth = dm.widthPixels;
		mScreenHeight = dm.heightPixels;
	}

	@Override
	public void surfaceChanged(SurfaceHolder arg0, int arg1, int arg2, int arg3) {
		// TODO Auto-generated method stub

	}

	@Override
	public void surfaceCreated(SurfaceHolder arg0) {
		// TODO Auto-generated method stub
		drawLogo();
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder arg0) {
		// TODO Auto-generated method stub
		if (isConnected) {
			try {
				inputstream.close();
				conn.disconnect();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}	
			Log.d(DEBUG_MSG, "close mysurface connection");
			System.exit(0);//这里如果不退出，还会继续绘图，这样就出现异常
		}
		
	}

	public void startCamera(String CameraIP) {
		this.CameraIP = CameraIP;
		isConnected = true;
		new DrawVideo().start();
	}

	public void freeCamera() {
		isConnected = false;
	}
 
	private void drawLogo() {
		Resources res = getResources();
		bm = BitmapFactory.decodeResource(res, R.drawable.wificar);
		mBitmap = Bitmap.createScaledBitmap(bm, mScreenWidth, mScreenHeight,
				true);
		canvas = sfh.lockCanvas();
		canvas.drawColor(Color.WHITE);
		canvas.drawBitmap(mBitmap, 0, 0, null);// 把BMP图像画在画布上
		sfh.unlockCanvasAndPost(canvas);// 画完一副图像，解锁画布
	}

	class DrawVideo extends Thread {

		public DrawVideo() {

		}

		@Override
		public void run() {
			while (isConnected) {
				
				try {
					videoURL = new URL(CameraIP);// 随时可以连上，只要IP有视频数据
					conn = (HttpURLConnection) videoURL.openConnection();
					conn.connect();
					inputstream = conn.getInputStream();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
					continue;
				} // 获取流
				
				bm = BitmapFactory.decodeStream(inputstream);// 从获取的流中构建出BMP图像
				mBitmap = Bitmap.createScaledBitmap(bm, mScreenWidth,
						mScreenHeight, true);
				canvas = sfh.lockCanvas();
				canvas.drawColor(Color.WHITE);
				canvas.drawBitmap(mBitmap, 0, 0, null);// 把BMP图像画在画布上
				sfh.unlockCanvasAndPost(canvas);// 画完一副图像，解锁画布
			}
			try {
				inputstream.close();
				conn.disconnect();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			drawLogo();	
		}
	}
}
