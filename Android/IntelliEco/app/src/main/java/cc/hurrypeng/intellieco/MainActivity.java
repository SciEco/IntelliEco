package cc.hurrypeng.intellieco;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.FileProvider;

import android.app.ProgressDialog;
import android.content.Intent;
import android.graphics.fonts.FontStyle;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.provider.MediaStore;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.google.gson.Gson;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

public class MainActivity extends AppCompatActivity {

    EditText editTextUsername;
    EditText editTextPassword;
    Button buttonLogin;

    ProgressDialog progressDialog;

    File request;
    File response;

    Handler handler;

    final int REQUEST_TAKE_PHOTO = 1;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        editTextUsername = findViewById(R.id.EditTextUsername);
        editTextPassword = findViewById(R.id.EditTextPassword);
        buttonLogin = findViewById(R.id.ButtonLogin);

        progressDialog = new ProgressDialog(MainActivity.this);
        progressDialog.setTitle("Loading");
        progressDialog.setCancelable(false);

        request = new File(getExternalCacheDir() + "/request.json");
        response = new File(getExternalCacheDir() + "/response.json");

        handler = new TaskHandler();

        /*
        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());
        */

        buttonLogin.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                DataPack.Login loginPack = new DataPack.Login();
                loginPack.username = editTextUsername.getText().toString();
                loginPack.password = editTextPassword.getText().toString();
                loginPack.request = "login";

                try {
                    if (request.exists()) request.delete();
                    request.createNewFile();

                    FileOutputStream requestStream = new FileOutputStream(request);
                    requestStream.write(new Gson().toJson(loginPack).getBytes());
                    requestStream.close();

                    progressDialog.setTitle("Logging in");
                    progressDialog.show();
                    FTPTask ftpTask = new FTPTask(handler, response, request);
                    ftpTask.start();
                }
                catch (Exception e) {
                    e.printStackTrace();
                }
                /*
                String imagePath, imageName;
                imagePath = getExternalCacheDir() + "/cache.jpg";
                imageName = imagePath.substring(imagePath.lastIndexOf('/') + 1, imagePath.length());
                File image = new File(imagePath);
                try {
                    if (image.exists()) image.delete();
                    image.createNewFile();
                } catch (IOException e) {
                    e.printStackTrace();
                }
                Uri imageUri;
                if (Build.VERSION.SDK_INT >= 24) {
                    imageUri = FileProvider.getUriForFile(MainActivity.this,
                            "cc.hurrypeng.intellieco.fileprovider", image);
                } else {
                    imageUri = Uri.fromFile(image);
                }

                Intent intent = new Intent("android.media.action.IMAGE_CAPTURE");
                intent.putExtra(MediaStore.EXTRA_OUTPUT, imageUri);
                startActivityForResult(intent, REQUEST_TAKE_PHOTO);
                */
            }
        });
    }

    class TaskHandler extends Handler {
        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);

            switch (msg.what) {
                case FTPTask.RESPONSE_RECV: {
                    try {
                        progressDialog.dismiss();
                        String responseContent = Util.getContent(response);
                        if (responseContent.equals("")) {
                            Toast.makeText(MainActivity.this, "FTP connection failed! ", Toast.LENGTH_LONG).show();
                        }

                        DataPack.LoginResponse loginResponse = new Gson().fromJson(responseContent, DataPack.LoginResponse.class);
                        Toast.makeText(MainActivity.this, loginResponse.authority, Toast.LENGTH_LONG).show();
                    }
                    catch (Exception e) {
                        e.printStackTrace();
                    }
                    break;
                }
            }
        }
    }
}
