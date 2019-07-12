package cc.hurrypeng.intellieco;

import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.Intent;
import android.location.Location;
import android.location.LocationManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;

import com.google.android.material.floatingactionbutton.FloatingActionButton;
import com.google.android.material.snackbar.Snackbar;
import com.google.gson.Gson;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.core.content.FileProvider;

import android.provider.MediaStore;
import android.view.View;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

public class RecordActivity extends AppCompatActivity {

    ProgressDialog progressDialog;

    FloatingActionButton fab;

    String username;
    String password;

    File request;
    File response;
    File sampleImage;

    LocationManager locationManager;

    final int REQUEST_TAKE_PHOTO = 1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_record);
        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        fab = findViewById(R.id.fab);

        progressDialog = new ProgressDialog(this);
        progressDialog.setTitle("Loading");
        progressDialog.setCancelable(false);

        username = getIntent().getStringExtra("username");
        password = getIntent().getStringExtra("password");

        request = new File(getExternalCacheDir() + "/request.json");
        response = new File(getExternalCacheDir() + "/response.json");
        sampleImage = new File(getExternalCacheDir() + "/cache.jpg");

        locationManager = (LocationManager) getSystemService(Context.LOCATION_SERVICE);

        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                try {
                    if (sampleImage.exists()) sampleImage.delete();
                    sampleImage.createNewFile();
                } catch (IOException e) {
                    e.printStackTrace();
                }
                Uri imageUri;
                if (Build.VERSION.SDK_INT >= 24) {
                    imageUri = FileProvider.getUriForFile(RecordActivity.this,
                            "cc.hurrypeng.intellieco.fileprovider", sampleImage);
                } else {
                    imageUri = Uri.fromFile(sampleImage);
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
                    progressDialog.setMessage("Uploading");
                    progressDialog.show();

                   try {
                       if (request.exists()) request.delete();
                       request.createNewFile();

                       final DataPack.UploadRequest uploadRequest = new DataPack.UploadRequest();
                       uploadRequest.username = username;
                       uploadRequest.password = password;
                       uploadRequest.request = "upload";
                       uploadRequest.time = System.currentTimeMillis();
                       Location location = locationManager.getLastKnownLocation(LocationManager.GPS_PROVIDER);
                       uploadRequest.longtitude = location.getLongitude();
                       uploadRequest.latitude = location.getLatitude();
                       uploadRequest.imageFilename = "cache.jpg";

                       FileOutputStream requestStream = new FileOutputStream(request);
                       requestStream.write(new Gson().toJson(uploadRequest).getBytes());
                       requestStream.close();

                       new FTPTask(response, request, sampleImage) {
                           @Override
                           void onPostExecute() {
                               super.onPostExecute();
                               progressDialog.dismiss();
                               try {
                                   String responseContent = Util.getContent(response);
                                   if (responseContent.equals("")) {
                                       Snackbar.make(fab, "FTP connection failed! ", Snackbar.LENGTH_LONG).show();
                                   }
                                   DataPack.Response generalResponse = new Gson().fromJson(responseContent, DataPack.Response.class);
                                   if (generalResponse.authority.equals("unauthorised")) Snackbar.make(fab, "Unauthorised! ", Snackbar.LENGTH_LONG).show();
                                   else {
                                       DataPack.UploadResponse uploadResponse = new Gson().fromJson(responseContent, DataPack.UploadResponse.class);
                                       AlertDialog.Builder dialog = new AlertDialog.Builder(RecordActivity.this);
                                       dialog.setTitle("Analyse Result");
                                       dialog.setMessage(uploadResponse.mothCount + " moth(s) have been counted. ");
                                       dialog.setCancelable(true);
                                       dialog.show();
                                   }
                               }
                               catch (IOException e) {
                                   e.printStackTrace();
                               }
                           }
                       }.execute();
                   }
                   catch (SecurityException e) {
                       e.printStackTrace();
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
