package cc.hurrypeng.ftptest;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;

import java.io.FileInputStream;
import java.io.IOException;
import java.net.InetSocketAddress;

import org.apache.commons.net.ftp.FTPClient;
import org.apache.commons.net.ftp.FTPReply;

import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

public class MainActivity extends AppCompatActivity {

    EditText editTextServer;
    EditText editTextUsername;
    EditText editTextPassword;
    Button buttonSelectFile;

    final int REQUEST_CHOOSEFILE = 1;

    String filePath;

    private class FtpTask extends AsyncTask<String, String, String> {
        @Override
        protected String doInBackground(String... strings) {
            new Thread(new Runnable() {
                @Override
                public void run() {

                    FTPClient ftp = new FTPClient();
                    try {
                        int reply;
                        String server = editTextServer.getText().toString();
                        ftp.connect(server);

                        // After connection attempt, you should check the reply code to verify
                        // success.
                        reply = ftp.getReplyCode();

                        if (!FTPReply.isPositiveCompletion(reply)) {
                            ftp.disconnect();
                            Log.e("TAG", "FTP server refused connection.");
                        }

                        ftp.login(editTextUsername.getText().toString(), editTextPassword.getText().toString());

                        ftp.enterLocalPassiveMode();
                        ftp.setRemoteVerificationEnabled(false);

                        //ftp.makeDirectory("/newdir");
                        ftp.storeFile("1.jpg", new FileInputStream(filePath));
                        //ftp.retrieveFile("/6.jpeg",new FileOutputStream("/storage/emulated/0/Android/data/cc.hurrypeng.ftptest/files/6.jpg"));
                        // ... // transfer files
                        //ftp.logout();
                    } catch (IOException e) {
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

        editTextServer = findViewById(R.id.EditTextServer);
        editTextUsername = findViewById(R.id.EditTextUsername);
        editTextPassword = findViewById(R.id.EditTextPassword);
        buttonSelectFile = findViewById(R.id.ButtonSelectFile);

        if (savedInstanceState != null)
        {
            editTextServer.setText(savedInstanceState.getString("server"));
            editTextUsername.setText(savedInstanceState.getString("username"));
            editTextPassword.setText(savedInstanceState.getString("password"));
        }

        buttonSelectFile.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
                intent.setType("*/*");
                intent.addCategory(Intent.CATEGORY_OPENABLE);
                startActivityForResult(intent, REQUEST_CHOOSEFILE);
            }
        });

    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (resultCode == RESULT_OK) {
            switch (requestCode) {
                case REQUEST_CHOOSEFILE: {
                    Uri uri = data.getData();
                    filePath = FileChooseUtil.getInstance(this).getChooseFileResultPath(uri);

                    new FtpTask().execute();
                }
            }
        }
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        outState.putString("server", editTextServer.getText().toString());
        outState.putString("username", editTextUsername.getText().toString());
        outState.putString("password", editTextPassword.getText().toString());
    }
}
