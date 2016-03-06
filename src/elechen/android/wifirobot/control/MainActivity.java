package elechen.android.wifirobot.control;

import java.io.IOException;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;

import elechen.android.wifirobot.R;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.view.Window;
import android.view.WindowManager;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.ImageButton;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.RadioGroup.OnCheckedChangeListener;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.Toast;

public class MainActivity extends Activity {
	MySurfaceView mySurfaceView;
	ImageButton imageButtomUp;
	ImageButton imageButtonDown;
	ImageButton imageButtonRight;
	ImageButton imageButtonLeft;
	ImageButton imageButtonExit;
	ImageButton imageButtonPlay;
	SeekBar seekBarSpeed;
	SeekBar seekBarCameraDirection;
	RadioButton radioButtonHandCtrl;
	RadioGroup radioGroup;
	CheckBox checkBoxOpenVideo;

	private int MAX_SPEED = 9;
	private int MAX_ANGLE = 180;
	private int HAND_CTRL_MODE = 0;
	private int VOICE_CTRL_MODE = 1;
	private int GRAVITY_CTRL_MODE = 2;

	private boolean isfirsttime = true;

	private boolean isConnected = false;

	private int CtrlMode = HAND_CTRL_MODE;// 0�ֿ�,1���أ�2������Ӧ
	private String DEBUG_MSG = "xf_debug";
	private Socket socket;
	private PrintWriter printWriter;
	private String CameraIP = "http://192.168.1.1:8080/?action=snapshot";// �趨��Ƶ��������ַ

	SensorManager sensorManager;
	SensorEventListener sensorEventListener;

	MyWiFiConnection myWiFiConnection;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);// ��ȥ����
		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);// ǿ�ƺ�����ʾ
		this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);
		setContentView(R.layout.main);
		initialGUI();
		initialAccelerometer();

		if (!WiFiIsOpened() && isfirsttime) {
			isfirsttime = false;
			Dialog alertDialog = new AlertDialog.Builder(this)
					.setTitle("��⵽WiFIû�д�").setMessage("���ȵ�ϵͳ��������С��WiFi")
					.setIcon(R.drawable.ic_launcher)
					.setNeutralButton("����ȥ����", new DialogInterface.OnClickListener() {
						public void onClick(DialogInterface dialog,
								int which) {
							// TODO Auto-generated method stub
							Intent wifiSettingsIntent = new Intent("android.settings.WIFI_SETTINGS");   
							startActivity(wifiSettingsIntent);  
						}
					})
					.create();
			alertDialog.show();
		}
	}

	// �˵�
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// TODO Auto-generated method stub
		menu.add(Menu.NONE, Menu.FIRST + 1, 1, "����").setIcon(
				android.R.drawable.ic_menu_help);
		menu.add(Menu.NONE, Menu.FIRST + 2, 2, "����").setIcon(
				android.R.drawable.ic_menu_info_details);
		menu.add(Menu.NONE, Menu.FIRST + 3, 3, "�˳�").setIcon(
				android.R.drawable.ic_menu_close_clear_cancel);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// TODO Auto-generated method stub
		switch (item.getItemId()) {
		case Menu.FIRST + 1:
			Toast.makeText(this, "����������", Toast.LENGTH_LONG).show();
			break;
		case Menu.FIRST + 2:
			Toast.makeText(this, "��ҵ���_����С��", Toast.LENGTH_LONG).show();
			break;
		case Menu.FIRST + 3:
			System.exit(0);
			break;
		default:
			break;
		}
		return false;
	}

	// ��ʼ����������пؼ�
	private void initialGUI() {
		mySurfaceView = (MySurfaceView) findViewById(R.id.mySurfaceView1); // ʵ����һ��SurfaceView�ؼ�

		imageButtomUp = (ImageButton) findViewById(R.id.imageButtonUp);
		imageButtonDown = (ImageButton) findViewById(R.id.imageButtonDown);
		imageButtonExit = (ImageButton) findViewById(R.id.imageButtonExit);
		imageButtonLeft = (ImageButton) findViewById(R.id.imageButtonLeft);
		imageButtonRight = (ImageButton) findViewById(R.id.imageButtonRight);
		imageButtonPlay = (ImageButton) findViewById(R.id.imageButtonPlay);
		MyButtonListener myButtonListener = new MyButtonListener();
		imageButtomUp.setOnClickListener(myButtonListener);
		imageButtomUp.setOnTouchListener(myButtonListener);
		imageButtonDown.setOnClickListener(myButtonListener);
		imageButtonDown.setOnTouchListener(myButtonListener);
		imageButtonExit.setOnClickListener(myButtonListener);
		imageButtonLeft.setOnClickListener(myButtonListener);
		imageButtonLeft.setOnTouchListener(myButtonListener);
		imageButtonPlay.setOnClickListener(myButtonListener);
		imageButtonRight.setOnClickListener(myButtonListener);
		imageButtonRight.setOnTouchListener(myButtonListener);

		seekBarCameraDirection = (SeekBar) findViewById(R.id.seekBarCameraDirction);
		seekBarSpeed = (SeekBar) findViewById(R.id.seekBarSpeed);
		seekBarSpeed.setMax(MAX_SPEED);// �ٶ�10��
		seekBarSpeed.setProgress(MAX_SPEED / 2);
		seekBarCameraDirection.setMax(MAX_ANGLE);// �Ƕȱ仯��Χ180
		seekBarCameraDirection.setProgress(MAX_ANGLE / 2);
		MySeekBarListener mySeekBarListener = new MySeekBarListener();
		seekBarCameraDirection.setOnSeekBarChangeListener(mySeekBarListener);
		seekBarSpeed.setOnSeekBarChangeListener(mySeekBarListener);

		radioButtonHandCtrl = (RadioButton) findViewById(R.id.radioButtonHandCtrl);
		radioGroup = (RadioGroup) findViewById(R.id.radioGroup);
		radioButtonHandCtrl.setChecked(true);// Ĭ�����ֿ�
		MyRadioGroupListener myRadioGroupListener = new MyRadioGroupListener();
		radioGroup.setOnCheckedChangeListener(myRadioGroupListener);
		
		checkBoxOpenVideo = (CheckBox)findViewById(R.id.checkBoxOpenVideo);
		MyCheckBoxListener myCheckBoxListener = new MyCheckBoxListener();
		checkBoxOpenVideo.setOnCheckedChangeListener(myCheckBoxListener);
		
	}

	public String speedLevel = "level5";
	public int cameraAngle = MAX_ANGLE / 2;

	public class MySeekBarListener implements OnSeekBarChangeListener {

		@Override
		public void onProgressChanged(SeekBar seekBar, int progress,
				boolean fromUser) {
			// TODO Auto-generated method stub
			switch (seekBar.getId()) {
			case R.id.seekBarSpeed:
				// ���ó����ٶ�
				speedLevel = "level" + seekBar.getProgress();
				break;
			case R.id.seekBarCameraDirction:
				// ��������ͷ�ĽǶ�
				cameraAngle = seekBar.getProgress();
				String tmpString = "";
				if (cameraAngle < 10) {
					tmpString = "00" + cameraAngle;

				} else if (cameraAngle < 100) {
					tmpString = "0" + cameraAngle;
				} else {
					tmpString = "" + cameraAngle;
				}
				sendCtrMessage("camera789" + tmpString);// ͳһ����12���ֽ�
				break;
			default:
				break;
			}
		}

		@Override
		public void onStartTrackingTouch(SeekBar seekBar) {
			// TODO Auto-generated method stub

		}

		@Override
		public void onStopTrackingTouch(SeekBar seekBar) {
			// TODO Auto-generated method stub

		}

	}
	
	public class MyCheckBoxListener implements android.widget.CompoundButton.OnCheckedChangeListener{

		@Override
		public void onCheckedChanged(CompoundButton buttonView,
				boolean isChecked) {
			// TODO Auto-generated method stub
			if(isChecked)
			{
				if(isConnected)
				{
					mySurfaceView.startCamera(CameraIP);
				}else {
					checkBoxOpenVideo.setChecked(false);
					Toast.makeText(getApplicationContext(), "��������С��WIFI",
							Toast.LENGTH_SHORT).show();	
				}
				
			}else {
				if(isConnected)
				{
					mySurfaceView.freeCamera();
				}
			}
			
		}
		
	}

	public class MyRadioGroupListener implements OnCheckedChangeListener {

		@Override
		public void onCheckedChanged(RadioGroup group, int checkedId) {
			// TODO Auto-generated method stub
			if (WiFiIsOpened()) {
				if (group.getId() == R.id.radioGroup) {
					switch (checkedId) {
					case R.id.radioButtonGravityCtrl:
						// ������Ӧ����С��
						CtrlMode = GRAVITY_CTRL_MODE;
						Toast.makeText(getApplicationContext(),
								"����������Ӧ����ģʽ" + CtrlMode, Toast.LENGTH_SHORT)
								.show();
						checkBoxOpenVideo.setClickable(false);
						checkBoxOpenVideo.setChecked(false);
						break;
					case R.id.radioButtonVoiceCtrl:

						// ��������С��
						CtrlMode = VOICE_CTRL_MODE;
						Toast.makeText(getApplicationContext(),
								"��������ģʽ" + CtrlMode, Toast.LENGTH_SHORT).show();
						if (isConnected) {
							myWiFiConnection.free();// ���ⴴ�´�����
							if(checkBoxOpenVideo.isChecked())
							{
								mySurfaceView.freeCamera();
							}
							isConnected = false;
							imageButtonPlay.setImageResource(R.drawable.play);
							try {
								Thread.sleep(500);
								Log.d("xf",
										"wait 500 milliseconds to enter another activity");
							} catch (InterruptedException e) {
								// TODO Auto-generated catch block
								e.printStackTrace();
							}
						}
						radioButtonHandCtrl.setChecked(true);
						checkBoxOpenVideo.setClickable(true);
						checkBoxOpenVideo.setChecked(false);
						Intent intent = new Intent();
						intent.setClass(
								MainActivity.this,
								elechen.android.wifirobot.recognize.MainActivity.class);
						startActivity(intent);

						break;
					case R.id.radioButtonHandCtrl:
						// �ֶ�����С��
						if (CtrlMode != VOICE_CTRL_MODE) {

							Toast.makeText(getApplicationContext(),
									"�����ֿ�ģʽ" + CtrlMode, Toast.LENGTH_SHORT)
									.show();
						}
						CtrlMode = HAND_CTRL_MODE;
						checkBoxOpenVideo.setClickable(true);
						break;

					default:
						break;
					}
				}
			}// û�д�WIFI
			else {
				Toast.makeText(getApplicationContext(), "������ϵͳ\"����\"������С��WIFI",
						Toast.LENGTH_SHORT).show();
			}
		}
	}

	public class MyButtonListener implements OnTouchListener, OnClickListener {

		@Override
		public boolean onTouch(View v, MotionEvent event) {
			// TODO Auto-generated method stub
			if (CtrlMode == HAND_CTRL_MODE) {
				switch (v.getId()) {
				case R.id.imageButtonUp:
					if (event.getAction() == MotionEvent.ACTION_DOWN) {
						// forward
						sendCtrMessage("forward89" + speedLevel);// Ҫ��乻�ַ���Arduino���Ӷ�����ʱҪ�󣬷���������ʱ������
					} else if (event.getAction() == MotionEvent.ACTION_UP) {
						// stop
						sendCtrMessage("stop56789" + speedLevel);
					}
					break;
				case R.id.imageButtonDown:
					if (event.getAction() == MotionEvent.ACTION_DOWN) {
						// backward
						sendCtrMessage("backward9" + speedLevel);
					} else if (event.getAction() == MotionEvent.ACTION_UP) {
						// stop
						sendCtrMessage("stop56789" + speedLevel);
					}
					break;
				case R.id.imageButtonLeft:
					if (event.getAction() == MotionEvent.ACTION_DOWN) {
						// turn left
						sendCtrMessage("turnleft9" + speedLevel);
					} else if (event.getAction() == MotionEvent.ACTION_UP) {
						// stop
						sendCtrMessage("stop56789" + speedLevel);
					}
					break;
				case R.id.imageButtonRight:
					if (event.getAction() == MotionEvent.ACTION_DOWN) {
						// turn right
						sendCtrMessage("turnright" + speedLevel);
					} else if (event.getAction() == MotionEvent.ACTION_UP) {
						// stop
						sendCtrMessage("stop56789" + speedLevel);
					}
					break;

				default:
					break;
				}
			} else if (CtrlMode == VOICE_CTRL_MODE) {
				// ����
			} else if (CtrlMode == GRAVITY_CTRL_MODE) {
				// ������Ӧ
			}
			return false;
		}

		@Override
		public void onClick(View v) {
			// TODO Auto-generated method stub
			switch (v.getId()) {
			case R.id.imageButtonExit:
				// �˳�����
				System.exit(0);
				break;
			case R.id.imageButtonPlay:
				if (WiFiIsOpened())// �Ѿ���WIFI
				{
					// ������
					if (!isConnected) {
						myWiFiConnection = new MyWiFiConnection();
						myWiFiConnection.start();
						isConnected = true;
						imageButtonPlay.setImageResource(R.drawable.pause);
					} else {
						myWiFiConnection.free();// �ر�����
						isConnected = false;
						imageButtonPlay.setImageResource(R.drawable.play);
					}
				} else {
					Toast.makeText(getApplicationContext(),
							"������ϵͳ\"����\"������С��WIFI", Toast.LENGTH_SHORT).show();
				}
				break;
			default:
				break;
			}
		}
	}

	public class MyWiFiConnection extends Thread {
		public MyWiFiConnection() {
			// do nothing
		}

		@Override
		public void run() {
			try {				
				socket = new Socket("192.168.1.1", 2001);// ser2net������IP�˿�
				printWriter = new PrintWriter(socket.getOutputStream());
				Log.d(DEBUG_MSG, "connect to Car WIFI OK");
			} catch (UnknownHostException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				Log.d(DEBUG_MSG, "UnknownHostException");
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				Log.d(DEBUG_MSG, "IOException");
			}
		}

		public void free() {
			printWriter.close();
			try {
				socket.close();
				Log.d(DEBUG_MSG, "disconnect Car WIFI OK");
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				Log.d(DEBUG_MSG, "disconnect Car WIFI failed");
			}
		}
	}

	public void sendCtrMessage(String ctrMessage) {
		if (!isConnected) {
			return;
		}

		if (!socket.isClosed()) {
			printWriter.write(ctrMessage);
			printWriter.flush();
			Log.d(DEBUG_MSG, "send: " + ctrMessage);
		} else {
			Log.d(DEBUG_MSG, "not connect to WIFI");
		}
	}

	public void initialAccelerometer() {
		sensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
		Sensor accelSensor = sensorManager
				.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);

		sensorEventListener = new SensorEventListener() {
			float y, x, oldX, oldY, tmpX, tmpY;

			@SuppressWarnings("deprecation")
			@Override
			public void onSensorChanged(SensorEvent event) {
				oldX = x;
				oldY = y;

				// TODO Auto-generated method stub
				y = event.values[SensorManager.DATA_X];
				x = event.values[SensorManager.DATA_Y];
				// float z = event.values[SensorManager.DATA_Z];

				tmpX = oldX - x;
				if (tmpX < 0) {
					tmpX = -tmpX;
				}
				tmpY = oldY - y;
				if (tmpY < 0) {
					tmpY = -tmpY;
				}

				if (CtrlMode == GRAVITY_CTRL_MODE
						&& (tmpX >= 0.1 || tmpY >= 0.1)) {
					sendCtrMessage(carStatus(y, x));
				}
			}

			@Override
			public void onAccuracyChanged(Sensor sensor, int accuracy) {
				// TODO Auto-generated method stub

			}
		};

		sensorManager.registerListener(sensorEventListener, accelSensor,
				SensorManager.SENSOR_DELAY_NORMAL);
	}

	// ���ݴ����������ݣ����ؿ�����Ϣ
	public String carStatus(float x, float y) {
		String CtrMsg;
		String Speed;

		if (y > 2.0) {
			CtrMsg = "turnright";
			y = y - 2;
			Speed = setSpeed(y);
			return CtrMsg + Speed;
		} else if (y < -2.0) {
			y = -y;
			y = y - 2;
			Speed = setSpeed(y);
			CtrMsg = "turnleft9";
			return CtrMsg + Speed;
		} else if (x > 5.0) {
			x = x - 5;
			Speed = setSpeed(x);
			CtrMsg = "backward9";
			return CtrMsg + Speed;
		} else if (x < 3.0) {
			x = 3 - x;
			Speed = setSpeed(x);
			CtrMsg = "forward89";
			return CtrMsg + Speed;
		} else {
			CtrMsg = "stop56789";
			return CtrMsg + "level0";
		}
	}

	// ���������Χ0-2
	public String setSpeed(float angle) {
		if (angle > 0 && angle < 0.2) {
			return "level1";
		} else if (angle > 0.2 && angle < 0.4) {
			return "level2";
		} else if (angle > 0.4 && angle < 0.6) {
			return "level3";

		} else if (angle > 0.6 && angle < 0.8) {
			return "level4";

		} else if (angle > 0.8 && angle < 1.0) {
			return "level5";

		} else if (angle > 1.0 && angle < 1.2) {
			return "level6";

		} else if (angle > 1.2 && angle < 1.4) {
			return "level7";

		} else if (angle > 1.4 && angle < 1.6) {
			return "level8";
		} else
			return "level9";
	}

	// WIFI�Ѿ����򷵻�true
	private boolean WiFiIsOpened() {
		ConnectivityManager connManager = (ConnectivityManager) MainActivity.this
				.getSystemService(CONNECTIVITY_SERVICE);
		NetworkInfo networkInfo = connManager.getActiveNetworkInfo();
		if (networkInfo != null
				&& networkInfo.getType() == ConnectivityManager.TYPE_WIFI
				&& networkInfo.isConnected()) {
			return true;
		} else {
			{
				return false;
			}
		}
	}
}
