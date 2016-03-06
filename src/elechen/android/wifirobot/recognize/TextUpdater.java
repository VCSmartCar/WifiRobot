package elechen.android.wifirobot.recognize;

import android.widget.TextView;

class TextUpdater implements Runnable
{
	TextView update_textview;
	String update_text;
	
	public TextUpdater(TextView tv, String text)
	{
		update_textview = tv;
		update_text = text;
	}//construction 1
	public TextUpdater(TextView tv)
	{
		update_textview = tv;
	}//construction 2
	public TextView get_Update_textview() {
		return update_textview;
	}
	public void set_Update_textview(TextView update_textview) {
		this.update_textview = update_textview;
	}
	public String get_Update_text() {
		return update_text;
	}
	public void set_Update_text(String update_text) {
		this.update_text = update_text;
	}
	
	@Override
	public void run() {
		if ((update_textview != null) && (update_text != null))
			update_textview.setText(update_text);
	}
}//end class TextUpdate