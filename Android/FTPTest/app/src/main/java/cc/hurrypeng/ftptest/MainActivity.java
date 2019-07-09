package cc.hurrypeng.ftptest;

import androidx.appcompat.app.AppCompatActivity;

import android.os.AsyncTask;
import android.os.Bundle;

import java.io.FileInputStream;
import java.io.IOException;

import org.apache.commons.net.ftp.FTPClient;
import org.apache.commons.net.ftp.FTPReply;

import android.util.Log;

public class MainActivity extends AppCompatActivity {

    private static class FtpTask extends AsyncTask<String, String, String> {
        @Override
        protected String doInBackground(String... strings) {
            new Thread(new Runnable() {
                @Override
                public void run() {

                    FTPClient ftp = new FTPClient();
                    try {
                        int reply;
                        String server = "ie.hurrypeng.cc";
                        ftp.connect(server);

                        // After connection attempt, you should check the reply code to verify
                        // success.
                        reply = ftp.getReplyCode();

                        if (!FTPReply.isPositiveCompletion(reply)) {
                            ftp.disconnect();
                            Log.e("TAG","FTP server refused connection.");
                        }

                        ftp.login("HurryPeng", "hurrypeng");

                        ftp.enterLocalPassiveMode();
                        ftp.setRemoteVerificationEnabled(false);

                        //ftp.makeDirectory("/newdir");
                        ftp.storeFile("1.jpg", new FileInputStream("/storage/emulated/0/Android/data/cc.hurrypeng.ftptest/files/IMG_20190709_062214.jpg"));
                        //ftp.retrieveFile("/6.jpeg",new FileOutputStream("/storage/emulated/0/Android/data/cc.hurrypeng.ftptest/files/6.jpg"));
                        // ... // transfer files
                        //ftp.logout();
                    } catch(IOException e) {
                        e.printStackTrace();
                    }

                }
            }).run();
            return null;
        }
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        new FtpTask().execute();

    }
}
