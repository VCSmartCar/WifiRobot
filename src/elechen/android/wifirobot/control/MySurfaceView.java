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
	// ����ؼ�
	private SurfaceHolder sfh = null;
	private URL videoURL = null;
	private Bitmap bm = null;// ͼƬ
	private Bitmap mBitmap = null;
	private Canvas canvas = null;// ����
	private Paint paint = null;// ����
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
		paint = new Paint();// ��ʼ������
		paint.setAntiAlias(true);
		paint.setColor(Color.RED);
		this.setKeepScreenOn(true);
		setFocusable(true);
	}

	// ��ʼ��������С
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
			System.exit(0);//����������˳������������ͼ�������ͳ����쳣
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
		canvas.drawBitmap(mBitmap, 0, 0, null);// ��BMPͼ���ڻ�����
		sfh.unlockCanvasAndPost(canvas);// ����һ��ͼ�񣬽�������
	}

	class DrawVideo extends Thread {

		public DrawVideo() {

		}

		@Override
		public void run() {
			while (isConnected) {
				
				try {
					videoURL = new URL(CameraIP);// ��ʱ�������ϣ�ֻҪIP����Ƶ����
					conn = (HttpURLConnection) videoURL.openConnection();
					conn.connect();
					inputstream = conn.getInputStream();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
					continue;
				} // ��ȡ��
				
				bm = BitmapFactory.decodeStream(inputstream);// �ӻ�ȡ�����й�����BMPͼ��
				mBitmap = Bitmap.createScaledBitmap(bm, mScreenWidth,
						mScreenHeight, true);
				canvas = sfh.lockCanvas();
				canvas.drawColor(Color.WHITE);
				canvas.drawBitmap(mBitmap, 0, 0, null);// ��BMPͼ���ڻ�����
				sfh.unlockCanvasAndPost(canvas);// ����һ��ͼ�񣬽�������
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
