package cc.hurrypeng.ftptest;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.FileProvider;

import android.app.ProgressDialog;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

import org.apache.commons.net.ftp.FTP;
import org.apache.commons.net.ftp.FTPClient;
import org.apache.commons.net.ftp.FTPReply;

import android.provider.MediaStore;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import com.google.gson.Gson;

public class MainActivity extends AppCompatActivity {

    SharedPreferences sp;
    SharedPreferences.Editor spEdit;

    LocationManager locationManager;

    EditText editTextServer;
    EditText editTextUsername;
    EditText editTextPassword;
    Button buttonSelectFile;

    ProgressDialog progressDialog;

    final int REQUEST_TAKE_PHOTO = 1;
    Uri imageUri;

    String server;
    String username;
    String password;
    String imageFilePath;
    String imageFilename;

    Gson gson;

    private class FtpTask extends AsyncTask<String, String, String> {
        private String jsonPath;
        private boolean success = false;
        @Override
        protected void onPreExecute() {
            super.onPreExecute();

            progressDialog.setTitle("Packing Data Sample");
            progressDialog.show();

            try {
                long time = System.currentTimeMillis();
                Location location = locationManager.getLastKnownLocation(LocationManager.GPS_PROVIDER);
                double longtitude = location.getLongitude();
                double latitude = location.getLatitude();
                DataSample dataSample = new DataSample(username, time, longtitude, latitude, imageFilename);
                String json = gson.toJson(dataSample);
                jsonPath = getExternalCacheDir() + "/DataSample.json";
                File jsonFile = new File(jsonPath);
                if (jsonFile.exists()) jsonFile.delete();
                jsonFile.createNewFile();
                FileOutputStream fileOutputStream = new FileOutputStream(jsonFile);
                fileOutputStream.write(json.getBytes());
            }
            catch (SecurityException e)
            {
                e.printStackTrace();
            }
            catch (Exception e)
            {
                e.printStackTrace();
            }
            progressDialog.setTitle("Uploading Data Sample");
        }

        @Override
        protected String doInBackground(String... strings) {
            new Thread(new Runnable() {
                @Override
                public void run() {
                    FTPClient ftp = new FTPClient();
                    try {
                        int reply;
                        ftp.connect(server);

                        // After connection attempt, you should check the reply code to verify
                        // success.
                        reply = ftp.getReplyCode();

                        if (!FTPReply.isPositiveCompletion(reply)) {
                            ftp.disconnect();
                            Log.e("TAG", "FTP server refused connection.");
                        }

                        success = ftp.login(username, password);

                        ftp.enterLocalPassiveMode();
                        ftp.setRemoteVerificationEnabled(false);

                        ftp.makeDirectory("/users/" + username);
                        ftp.changeWorkingDirectory("/users/" + username);
                        ftp.setFileType(FTP.BINARY_FILE_TYPE);

                        ftp.storeFile("DataSample.json", new FileInputStream(jsonPath));
                        ftp.storeFile(imageFilename, new FileInputStream(imageFilePath));

                        ftp.makeDirectory("/users/" + username + "/uploadfin");
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

        @Override
        protected void onPostExecute(String s) {
            progressDialog.setTitle("Server Processing");
            progressDialog.dismiss();
            if (!success) Toast.makeText(MainActivity.this, "FTP operation failed! ", Toast.LENGTH_LONG).show();
            else Toast.makeText(MainActivity.this, "FTP upload successful", Toast.LENGTH_LONG).show();
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        sp = getSharedPreferences("ftp", MODE_PRIVATE);
        spEdit = sp.edit();

        server = sp.getString("server", "");
        username = sp.getString("username", "");
        password = sp.getString("password", "");

        editTextServer = findViewById(R.id.EditTextServer);
        editTextUsername = findViewById(R.id.EditTextUsername);
        editTextPassword = findViewById(R.id.EditTextPassword);
        buttonSelectFile = findViewById(R.id.ButtonSelectFile);

        editTextServer.setText(server);
        editTextUsername.setText(username);
        editTextPassword.setText(password);

        progressDialog = new ProgressDialog(MainActivity.this);
        progressDialog.setMessage("Loading...");
        progressDialog.setCancelable(false);

        gson = new Gson();

        locationManager = (LocationManager) getSystemService(Context.LOCATION_SERVICE);

        if (savedInstanceState != null)
        {
            editTextServer.setText(savedInstanceState.getString("server"));
            editTextUsername.setText(savedInstanceState.getString("username"));
            editTextPassword.setText(savedInstanceState.getString("password"));
        }

        buttonSelectFile.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                server = editTextServer.getText().toString();
                username = editTextUsername.getText().toString();
                password = editTextPassword.getText().toString();

                spEdit.putString("server", server);
                spEdit.putString("username", username);
                spEdit.putString("password", password);
                spEdit.apply();

                /*
                Intent intent = new Intent(Intent.ACTION_PICK, android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
                intent.addCategory(Intent.CATEGORY_OPENABLE);
                startActivityForResult(intent, REQUEST_CHOOSEFILE);
                */

                imageFilePath = getExternalCacheDir() + "/cache.jpg";
                imageFilename = imageFilePath.substring(imageFilePath.lastIndexOf('/') + 1, imageFilePath.length());
                File photo = new File(imageFilePath);
                try {
                    if (photo.exists()) photo.delete();
                    photo.createNewFile();
                } catch (IOException e) {
                    e.printStackTrace();
                }
                if (Build.VERSION.SDK_INT >= 24) {
                    imageUri = FileProvider.getUriForFile(MainActivity.this,
                            "cc.hurrypeng.ftptest.fileprovider", photo);
                } else {
                    imageUri = Uri.fromFile(photo);
                }

                Intent intent = new Intent("android.media.action.IMAGE_CAPTURE");
                intent.putExtra(MediaStore.EXTRA_OUTPUT, imageUri);
                startActivityForResult(intent, REQUEST_TAKE_PHOTO);
            }
        });

    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (resultCode == RESULT_OK) {
            switch (requestCode) {
                case REQUEST_TAKE_PHOTO: {
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
