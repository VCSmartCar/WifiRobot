package elechen.android.wifirobot.recognize;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;

import elechen.android.wifirobot.R;

import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.content.res.AssetManager;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity {

	private Button dhmm_button;
	private Button dtw_button;
	private Button sample_button;
	private Button forward_button;
	private Button backward_button;
	private Button left_button;
	private Button right_button;
	private Button stop_button;

	private int dtw_index;
	private boolean sample_recorded[] = { false, false, false, false, false };
	private String result_string[] = { "ǰ��", "����", "��ת", "��ת", "��ͣ" };
	private boolean isConnected = false;
	private TextView textView;
	private boolean dhmm_flag;
	private boolean dtw_flag;
	private boolean sampling_flag;
	private boolean isfinish_flag = false;
	private RecorderThread recorderThread;
	private AssetManager assetManager;
	private Handler my_handler;

	private final short DHMM_RECOG = 0;
	private final short DTW_RECOG = 1;
	PrintWriter out;

	private String model_file_path = "/mnt/sdcard/dhmm_data/";
	private String model_file_name = "test_dhmm.dat";
	private String codebook_file_path = model_file_path;
	private String codebook_file_name = "test_codebook.dat";
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);// ��ȥ����
		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);// ǿ�ƺ�����ʾ
		this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		
		setContentView(R.layout.activity_main);

		dhmm_button = (Button) findViewById(R.id.button_dhmm);
		dhmm_button.setOnClickListener(button_listener);

		dtw_button = (Button) findViewById(R.id.button_dtw);
		dtw_button.setOnClickListener(button_listener);

		sample_button = (Button) findViewById(R.id.button_sample);
		sample_button.setOnClickListener(button_listener);

		forward_button = (Button) findViewById(R.id.button_qian);
		forward_button.setOnClickListener(button_listener);
		forward_button.setVisibility(View.INVISIBLE);

		backward_button = (Button) findViewById(R.id.button_hou);
		backward_button.setOnClickListener(button_listener);
		backward_button.setVisibility(View.INVISIBLE);

		left_button = (Button) findViewById(R.id.button_zuo);
		left_button.setOnClickListener(button_listener);
		left_button.setVisibility(View.INVISIBLE);

		right_button = (Button) findViewById(R.id.button_you);
		right_button.setOnClickListener(button_listener);
		right_button.setVisibility(View.INVISIBLE);

		stop_button = (Button) findViewById(R.id.button_ting);
		stop_button.setOnClickListener(button_listener);
		stop_button.setVisibility(View.INVISIBLE);
		

		textView = (TextView) findViewById(R.id.textView_result);
		dhmm_flag = false;
		dtw_flag = false;
		sampling_flag = false;
		

		my_handler = new Handler();

		assetManager = getAssets();
		if (!is_datafile_exist())
		{
			//��������ļ������ڣ��ʹ�assets�����ļ���ȥsdcard
			copy_file(model_file_path, model_file_name);
			copy_file(codebook_file_path, codebook_file_name);
		}
        readHmm(model_file_path + model_file_name);
        readCodebook(codebook_file_path + codebook_file_name);
	}
	
	@Override
	protected void onStop() {
		// TODO Auto-generated method stub
		if((recorderThread!=null) && recorderThread.isAlive())
		{
			recorderThread.free();
		}
		super.onStop();
	}

	//���ģ�����뱾�������Ƿ��Ѿ�����
	private boolean is_datafile_exist()
	{
		File m_file = new File(model_file_path + model_file_name);
		File c_file = new File(codebook_file_path + codebook_file_name);
		if (m_file.exists() && c_file.exists())
			return true;
		else
			return false;
	}//end is_datafile_exist
		
	// ����asset�ļ���sdcard��
	public void copy_file(String path, String filename) {

		try {
			int buff_size = 512;
			int length;
			byte data[] = new byte[buff_size];

			InputStream is = assetManager.open(filename);
			File file = new File(path + filename);
			if (!file.getParentFile().exists())
				file.getParentFile().mkdir();
			// ����ļ������ڣ��½�һ��, ������ڣ��͸���
			if (!file.exists())
				file.createNewFile();

			FileOutputStream fos = new FileOutputStream(file, false);

			// ��������
			length = is.read(data);
			while (length != -1) {
				fos.write(data, 0, length);
				length = is.read(data);
			}

			// �ر�������
			is.close();
			fos.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}// end copy_file

	public native void checkDhmm();

	public native boolean readHmm(String path);

	public native boolean readCodebook(String path);

	public native void readMatrix(String path, int matrix[], int m, int n);

	static public native int recognize(int arr[], int size, short recog_type);

	static public native void readDtw(int model[], int size, int index);

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.activity_main, menu);
		return true;
	}

	private OnClickListener button_listener = new OnClickListener() {

		@Override
		public void onClick(View v) {
			switch (v.getId()) {
			case R.id.button_dhmm:
				if (dhmm_flag) {
					sample_button.setEnabled(true);
					dtw_button.setEnabled(true);

					dhmm_button.setText("��ͨ��ʶ��");
					dhmm_flag = false;

					recorderThread.free();
				} else {
					sample_button.setEnabled(false); // ʶ���ڼ䲻��¼����
					dtw_button.setEnabled(false);

					dhmm_button.setText("�ر�¼��");
					dhmm_flag = true;

					recorderThread = new RecorderThread();
					recorderThread.init();
					recorderThread.start();
				}
				break;
			case R.id.button_dtw:
				if (dtw_flag) {
					sample_button.setEnabled(true);
					dhmm_button.setEnabled(true);

					dtw_button.setText("����ʶ��");
					dtw_flag = false;
					recorderThread.free();
				} else {
					boolean all_sample = true;
					int i;
					for (i = 0; i < 5; i++) {
						all_sample &= sample_recorded[i];
					}

					// �������������������ʶ��
					if (!all_sample) {
						Toast.makeText(getApplicationContext(), "����������",
								Toast.LENGTH_LONG).show();
						break;
					}
					sample_button.setEnabled(false); // ʶ���ڼ䲻��¼����
					dhmm_button.setEnabled(false);

					dtw_button.setText("�ر�¼��");
					dtw_flag = true;

					recorderThread = new RecorderThread();
					recorderThread.init();
					recorderThread.start();
				}
				break;
			case R.id.button_sample:
				if (sampling_flag) {
					dhmm_button.setEnabled(true);
					dtw_button.setEnabled(true);
					
					//���¼��û�رգ��ȹر�
					if ((recorderThread!=null) && recorderThread.isAlive())
						recorderThread.free();
					textView.setText("");
					
					sample_button.setText("���������ɼ�");
					sampling_flag = false;

					// ������������¼����ť
					forward_button.setVisibility(View.INVISIBLE);
					backward_button.setVisibility(View.INVISIBLE);
					left_button.setVisibility(View.INVISIBLE);
					right_button.setVisibility(View.INVISIBLE);
					stop_button.setVisibility(View.INVISIBLE);
				} else {
					dhmm_button.setEnabled(false); // �����ɼ��ڼ䲻׼ʶ��
					dtw_button.setEnabled(false);
					
					sample_button.setText("��ɲɼ�");
					sampling_flag = true;

					// ��ʾ��������¼����ť
					forward_button.setVisibility(View.VISIBLE);
					backward_button.setVisibility(View.VISIBLE);
					left_button.setVisibility(View.VISIBLE);
					right_button.setVisibility(View.VISIBLE);
					stop_button.setVisibility(View.VISIBLE);
					
				}
				break;

			// ����¼������һ������¼ʱ��������һ����ʱ����ر����ڵ�¼��
			case R.id.button_qian:
				dtw_index = 0;
				textView.setText("����¼"+result_string[dtw_index]);
				Log.i("test", forward_button.getText().toString());
				Log.i("test", "�������Ϊ" + dtw_index);
				if (recorderThread != null) {
					if (recorderThread.isAlive())
					{
						recorderThread.free();
						Log.i("test", "free is executed");
					}
				}
				while ((recorderThread != null) && (!recorderThread.is_finish()));
				recorderThread = new RecorderThread();
				recorderThread.init();
				recorderThread.start();

				// ��¼����ɺ���Զ��ر�¼������recorderThread��ʵ��
				break;

			case R.id.button_hou:
				dtw_index = 1;
				textView.setText("����¼"+result_string[dtw_index]);
				Log.i("test", backward_button.getText().toString());
				Log.i("test", "�������Ϊ" + dtw_index);
				if (recorderThread != null) {
					if (recorderThread.isAlive())
						recorderThread.free();
				}
				while ((recorderThread != null) && (!recorderThread.is_finish()));
				recorderThread = new RecorderThread();
				recorderThread.init();
				recorderThread.start();
				break;
			case R.id.button_zuo:
				dtw_index = 2;
				textView.setText("����¼"+result_string[dtw_index]);
				Log.i("test", left_button.getText().toString());
				Log.i("test", "�������Ϊ" + dtw_index);
				if (recorderThread != null) {
					if (recorderThread.isAlive())
						recorderThread.free();
				}
				while ((recorderThread != null) && (!recorderThread.is_finish()));
				recorderThread = new RecorderThread();
				recorderThread.init();
				recorderThread.start();
				break;
			case R.id.button_you:
				dtw_index = 3;
				textView.setText("����¼"+result_string[dtw_index]);
				Log.i("test", right_button.getText().toString());
				Log.i("test", "�������Ϊ" + dtw_index);
				if (recorderThread != null) {
					if (recorderThread.isAlive())
						recorderThread.free();
				}
				while ((recorderThread != null) && (!recorderThread.is_finish()));
				recorderThread = new RecorderThread();
				recorderThread.init();
				recorderThread.start();
				break;
			case R.id.button_ting:
				dtw_index = 4;
				textView.setText("����¼"+result_string[dtw_index]);
				Log.i("test", stop_button.getText().toString());
				Log.i("test", "�������Ϊ" + dtw_index); 
				if (recorderThread != null) {
					if (recorderThread.isAlive())
						recorderThread.free();
				}
				while ((recorderThread != null) && (!recorderThread.is_finish()));
				recorderThread = new RecorderThread();
				recorderThread.init();
				recorderThread.start();
				break;
			default:
				break;
			}// end switch

		}// end onClick

	};

	/*
	 * this is used to load the 'hello-jni' library on application startup. The
	 * library has already been unpacked into
	 * /data/data/com.example.hellojni/lib/libhello-jni.so at installation time
	 * by the package manager.
	 */
	static {
		System.loadLibrary("hello-jni");
	}

	class RecorderThread extends Thread {
		protected AudioRecord audioRecord;
		protected int bufSizeR;
		private int sampleRateInHz = 8000;
		private int inChannelConfig = AudioFormat.CHANNEL_IN_MONO;
		private int audioFormat = AudioFormat.ENCODING_PCM_16BIT;
		private int audioSource = MediaRecorder.AudioSource.MIC;
		boolean keep_running = false;
		boolean isDebug = false;// isDebug means not use socket

		protected AudioTrack audioTrack;
		protected int bufSizeT;
		protected int streamType = AudioManager.STREAM_MUSIC;
		protected int mode = AudioTrack.MODE_STREAM;
		private int outChannelConfig = AudioFormat.CHANNEL_OUT_MONO;

		private String DEBUG_MSG = "xf_debug";

		private Pre_process pre_process = new Pre_process();
		private boolean first_frame_flag;
		private short frame_tmp1[] = new short[pre_process.get_frame_len()];
		private short frame_tmp2[] = new short[pre_process.get_frame_len()];
		private double frame_input[] = new double[pre_process.get_frame_len()];

		private int result; // ʶ����
		private String result_string[] = { "ǰ��", "����", "��ת", "��ת", "��ͣ" };

		private TextUpdater textUpdater = new TextUpdater(textView);
		
		public void init() {
			bufSizeR = AudioRecord.getMinBufferSize(sampleRateInHz,
					inChannelConfig, audioFormat);
			audioRecord = new AudioRecord(audioSource, sampleRateInHz,
					inChannelConfig, audioFormat, bufSizeR);
			isDebug = false;
			if (isDebug) {
				// ��ʵ�ֱ�¼�߷�
				bufSizeT = AudioTrack.getMinBufferSize(sampleRateInHz,
						outChannelConfig, audioFormat);
				audioTrack = new AudioTrack(streamType, sampleRateInHz,
						outChannelConfig, audioFormat, bufSizeT, mode);
				Log.d(DEBUG_MSG, "bufSizeT = " + bufSizeT);
			}
			keep_running = true;
			first_frame_flag = true;
			isfinish_flag = false;
		}

		@Override
		public void run() {
			int i, j;
			int index;
			int left;
			int data_count;
			int frame_len = pre_process.get_frame_len();
			int frame_shift = pre_process.get_frame_shift();
			
			if (!isConnected) {
				try {
					socket = new Socket("192.168.1.1", 2001);
					out = new PrintWriter(socket.getOutputStream());
					isConnected = true;
					Log.d("xf", "connect to wifi OK");
				} catch (UnknownHostException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
					Log.d("xf", "connect to wifi failed:UnknownHostException");
					isConnected = false;
					return;
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
					Log.d("xf", "connect to wifi failed:IOException");	
					isConnected = false;
					return;
				}
			}

			if (isDebug) {
				if (audioTrack.getState() == AudioTrack.STATE_INITIALIZED) {
					audioTrack.play();
					Log.d(DEBUG_MSG, "initial Track OK");
				} else {
					Log.d(DEBUG_MSG, "can't initial Track");
					return;
				}
				audioTrack.setStereoVolume(1.0f, 1.0f);
			}
			Log.d(DEBUG_MSG, "bufSizeR = " + bufSizeR);

			if (audioRecord.getState() == AudioRecord.STATE_INITIALIZED) {
				Log.d(DEBUG_MSG, "initial Recorder OK");
				audioRecord.startRecording();
			} else {
				Log.d(DEBUG_MSG, "can't initial Recorder");
				audioRecord.release();
				if (isDebug)
				{
					audioTrack.stop();
					audioTrack.release();
					Log.d(DEBUG_MSG, "release Track OK");
				}
				return;
			}

			pre_process.waveReset();
			while (keep_running) {

				if (first_frame_flag) {
					first_frame_flag = false;
					index = 0;
					left = frame_len;
					data_count = audioRecord.read(frame_tmp1, index, left);

					// �ȴ������㹻������
					while (data_count < left) {
						index += data_count;
						left -= data_count;
						data_count = audioRecord.read(frame_tmp1, index, left);
					}
					if (isDebug) {
						audioTrack.write(frame_tmp1, 0, frame_tmp1.length);
						// audioTrack.write(audioData, 0, 256);
					}
				} else {
					index = 0;
					left = frame_shift;
					/*
					 * fix note: ֮ǰ��Ϊ����һ��read��䣬����ʶ���ʵͣ�����¼��ʱ��Խ��ʶ����Խ��
					 */
					data_count = audioRecord.read(frame_tmp2, index, left);
					// �ȴ������㹻������
					while (data_count < left) {
						index += data_count;
						left -= data_count;
						data_count = audioRecord.read(frame_tmp2, index, left);
					}

					if (isDebug) {
						audioTrack.write(frame_tmp2, 0, frame_shift);
						// audioTrack.write(audioData, 0, 256);
					}
					// ������һ֡��һ��������
					for (i = frame_shift; i < frame_len; i++) {
						frame_tmp1[i - frame_shift] = frame_tmp1[i];
					}
					// ����������
					for (i = frame_len - frame_shift, j = 0; i < frame_len; i++, j++) {
						frame_tmp1[i] = frame_tmp2[j];
					}
				}
				// ��ΪС��
				for (i = 0; i < frame_len; i++) {
					frame_input[i] = frame_tmp1[i] / 32768.0;
				}
				if (pre_process.waveAppend(frame_input)) {
					if (dhmm_flag || dtw_flag) {
						// ʶ��׶Σ��ѷ�֡��������ó�����Ȼ��ʶ����ʾʶ����
						audioRecord.stop();

						first_frame_flag = true;
						pre_process.my_frames.delete_silence();
						pre_process.my_frames.normalize();
						if (dhmm_flag)
							result = MainActivity.recognize(
									pre_process.my_frames.get_frames(),
									pre_process.my_frames.get_size(),
									DHMM_RECOG);
						else
							result = MainActivity
									.recognize(
											pre_process.my_frames.get_frames(),
											pre_process.my_frames.get_size(),
											DTW_RECOG);
						// �������ָ��
						ctrlCar(result);

						if (result == -1)
							textUpdater.set_Update_text("��Ч¼��");
						else
							textUpdater.set_Update_text("ʶ��Ϊ" + result_string[result]);
						my_handler.post(textUpdater); // ��ʾʶ����

						pre_process.waveReset();

						audioRecord.startRecording();
					}

					else if (sampling_flag) {
						// ���������׶Σ��ѷ�֡��������ó������浽dtwģ�壬Ȼ�����¼��
						audioRecord.stop();

						pre_process.my_frames.delete_silence();
						pre_process.my_frames.normalize();
						MainActivity.readDtw(
								pre_process.my_frames.get_frames(),
								pre_process.my_frames.get_size(), dtw_index);

						textUpdater.set_Update_text(result_string[dtw_index]+"¼�����");
						my_handler.post(textUpdater);

						pre_process.waveReset();

						keep_running = false;

						sample_recorded[dtw_index] = true; // ���Ϊ��¼����Ҫȫ��������¼���ſ�����dtwʶ��
						Log.i("test", "�������Ϊ" + dtw_index);
					} else {
						keep_running = false;
						Log.i("test", "����");
					}
				}// end if the end of �˵���
			}// end while keep_running

			// ¼��������Ĵ���
			audioRecord.stop();
			audioRecord.release();
			Log.d(DEBUG_MSG, "release Rocorder OK");
			
			try {
				out.close();
				socket.close();
				isConnected = false;
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			if (isDebug) {
				audioTrack.stop();
				audioTrack.release();
				Log.d(DEBUG_MSG, "release Track OK");
			}
			isfinish_flag = true;
		}

		// called when this APP closing
		public void free() {
			keep_running = false;
		}
		
		public boolean is_finish()
		{
			return isfinish_flag;
		}//end is_finish

	}// end RecorderThread

	Socket socket;

	public void sendCtrMessage(String ctrMessage) {
		try {
			out.write(ctrMessage);
			out.flush();
			//socket.close();
			Log.d("xf", "���ͳɹ���" + ctrMessage);
		} catch (Exception e) {
			Log.d("xf", "����ʧ�ܣ�");
		}
	}

	// ��������ʶ��������С��
	public void ctrlCar(int result) {
		if (result < 0 && result > 4) {
			Log.d("xf", "����ʶ������");
			return;
		}
		switch (result) {
		case 0:
			sendCtrMessage("forward89level6");
			break;
		case 1:
			sendCtrMessage("backward9level6");
			break;
		case 2:
			sendCtrMessage("turnleft9level6");
			break;
		case 3:
			sendCtrMessage("turnrightlevel6");
			break;
		case 4:
			sendCtrMessage("stop56789123456");
			break;
		default:
			break;
		}
	}
}// end class MainActivity
