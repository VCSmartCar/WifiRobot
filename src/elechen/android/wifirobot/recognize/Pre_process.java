package elechen.android.wifirobot.recognize;

public class Pre_process {

	private final int FRAME_LEN = 256;
	private final int FRAME_SHIFT = 80;
	private final int FRAMES = 120;
	private final int MAX_SAMPLES = 96000;  //帧数据缓冲的最大长度
	private final int MAXLEN = 20000;		//语音数据缓冲的最大长度
	private boolean flag_word_start;
	private boolean flag_word_end;
	private boolean flag_poss_start;
	private boolean flag_user_stop;

	private int max_silence_time;
	private int min_word_length;
	private int silence_time;
	private int word_length;
	private double amp, amp_high, amp_low;//, amp_buf[FRAMES];
	private double zcr, zcr_high, zcr_low;//, zcr_buf[FRAMES];
	private double zcr_threshold;





	private double tmp1 = 0;
	private double tmp2 = 0; 
 	
	public Frames my_frames = new Frames();
 	
	public void waveReset()
	{
	 	flag_word_start = false;
	 	flag_poss_start = false;
	 	flag_word_end = false;
	 	flag_user_stop = false;
	 	
	 	amp_high = 10;	//语音确认开始能量门限
	 	amp_low = 1;		//语音可能开始能量门限
	 	zcr_high = 10;	//语音确信开始过零率门限
	 	zcr_low = 1;		//语音可能开始过零率门限
	 	zcr_threshold = 0.02;	
	 	
	 	min_word_length = 35;	//最短帧数
	 	max_silence_time = 10;	//最长静音时间
	 	silence_time = 0;		//静音时间
	 	word_length = 0;		//语音帧数 	
	 	
	 	tmp1 = 0;
	 	tmp2 = 0;
	 	
	 	my_frames.clear();  //之前没有这句，导致上一个有效录音的帧还存留着，导致识别率差
	 }//end waveReset
	
	 //预加重滤波器

	 private void waveFilter(double buff[])
	 {
	 	int i;
	 	for (i = 0; i < FRAME_LEN; i++)
	 	{
	 		tmp1 = buff[i] - tmp2 * 0.9375;
	 		tmp2 = buff[i];
	 		buff[i] = tmp1;
	 	}
	 }//end waveFilter
	 //计算一帧短时能量
	 private void waveEnergy(double buff[])
	 {
	 	int i = 0;
	 	amp = 0.0;
	 	for (i = 0; i < FRAME_LEN; i++)
	 	{
	 		amp += Math.abs(buff[i]);
	 	}
	 	
	 	//if (word_length > 0)
	 	//	amp_buf[word_length - 1] = amp;
	 }//end waveEnergy
	 //计算一帧过零率
	 private void waveZcr(double buff[])
	 {
	 	int i = 0;
	 	zcr = 0;
	 	for (i = 0; i < FRAME_LEN - 1; i++)
	 	{
	 		double tmp1 = buff[i];
	 		double tmp2 = buff[i+1];
	 		if ((tmp1 * tmp2 < 0) && (Math.abs(tmp1 - tmp2) > zcr_threshold))
	 			zcr++;
	 	}
	 	//if (word_length > 0)
	 	//	zcr_buf[word_length - 1] = zcr;
	 }
	 //端点检测，如果是有效录音，返回true，否则返回false
	 private boolean waveVad()
	 {
		 boolean return_value;
		if (flag_word_start)
		{
			//检查语音结束
			if (amp >= amp_low || zcr >= zcr_low)
			{
				//还没有结束
				silence_time = 0;
				word_length++;
				return_value = true;
				
				//语音中间的暂停
				if (word_length >= FRAMES - 3)
				{
					word_length = FRAMES - 3;
					flag_word_start = false;
					flag_poss_start = false;
					flag_word_end = true;		//缓冲区满了
				}
			}
			else
			{
				silence_time++;
				word_length++;
				//静音时间不够长
				if (silence_time < max_silence_time)
				{
					return_value = true;
					//语音中间的暂停
					if (word_length >= FRAMES - 3)
					{
						word_length = FRAMES - 3;
						flag_word_start = false;
						flag_poss_start = false;
						flag_word_end = true;		//缓冲区满了
					}
				}
				//静音时间够长，如果帧数够的话，有效录音结束，如果不够，不是有效录音
				else if (word_length >= min_word_length)
				{
					return_value = true;
					flag_word_start = false;
					flag_poss_start = false;
					flag_word_end = true;		
				}
				else
				{
					//无效录音
					return_value = false;
					//语音过短
					flag_poss_start = false;
					flag_word_start = false;
					flag_word_end = false;
					silence_time = 0;
					word_length = 0;
				}
			}
			
		}
		else
		{
			//检查语音开始
			if (amp >= amp_high || zcr >= zcr_high)
			{
				return_value = true;
				flag_word_start = true;
				flag_poss_start = false;
				silence_time = 0;
				word_length++;
			}
			else if (amp >= amp_low || zcr >= zcr_low)
			{
				return_value = true;
				//可能是语音开始
				flag_poss_start = true;
				word_length++;
				if (word_length >= FRAMES - 3)
				{
					word_length = FRAMES - 3;
					flag_word_start = false;
					flag_poss_start = false;
					flag_word_end = true;		//缓冲区满了
				}
			}
			else
			{
				return_value = false;
				//语音未开始
				flag_poss_start = false;
				flag_word_start = false;
				flag_word_end = false;
				silence_time = 0;
				word_length = 0;
			}
		}//end if
		return return_value;
	 }//end wavevad
	 
	//处理一个帧
	public boolean waveAppend(double frame[])
	{
		waveZcr(frame);
		waveFilter(frame);
		waveEnergy(frame);
	
		if (waveVad())
		{
			my_frames.append(frame);
		}
		else
			my_frames.clear();
		return flag_word_end;
	}

	

	
	public int get_frame_len()
	{
		return FRAME_LEN;
	}
	
	public int get_frame_shift()
	{
		return FRAME_SHIFT;
	}
	



	class Frames
	{
		private int frames[] = new int[MAX_SAMPLES];
		private int start = 0;
		private int size = 0;
		//清空frames数组
		public void clear()
		{
			start = 0;
			size = 0;
		}
		//加入一个新的帧
		public void append(double frame[])
		{
			int i;
			for (i = 0; i < FRAME_LEN; i++)
			{
				frames[start + i] = (int)(frame[i] * ((long)1 << 31));
			}//end for
			start += FRAME_LEN;
			size++;
		}
		//删除静音时间
		public void delete_silence()
		{
			if (flag_word_end)
			{
				size -= silence_time;
			}
		}//end delete_silence
		//归一化
		public void normalize()
		{
			int i, j;
			int temp;
			int max;
			//求出最大值
			max = Math.abs(frames[0]);
			for (i = 1; i < FRAME_LEN; i++)
			{
				if (max < Math.abs(frames[i]))
					max = Math.abs(frames[i]);
			}//end for i
			for (i = 1; i < size; i++)
			{
				for (j = FRAME_SHIFT; j < FRAME_LEN; j++)
				{
					temp = Math.abs(frames[i * FRAME_LEN + j]);
					if (max < temp)
						max = temp;
				}//end for j
			}//end for i
			
			//归一
			for (i = 0; i < size; i++)
			{
				for (j = 0; j < FRAME_LEN; j++)
				{
					temp = i * FRAME_LEN + j;
					frames[temp] = (int) (((long)frames[temp]<<31) / max);
				}//end for j
			}//end for i
		}//end normalize
		
		//获取帧数组
		public int []get_frames()
		{
			return frames;
		}
		public int get_size()
		{
			return size;
		}
	}//end class Frames
}
