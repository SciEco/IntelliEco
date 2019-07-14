package cc.hurrypeng.intellieco;

import androidx.appcompat.app.AppCompatActivity;

import android.app.ProgressDialog;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import com.google.android.material.snackbar.Snackbar;
import com.google.gson.Gson;

import java.io.File;
import java.io.FileOutputStream;

public class MainActivity extends AppCompatActivity {

    EditText editTextUsername;
    EditText editTextPassword;
    Button buttonLogin;

    ProgressDialog progressDialog;

    File request;
    File response;

    String username;
    String password;

    SharedPreferences sp;
    SharedPreferences.Editor spEdit;

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

        progressDialog = new ProgressDialog(this);
        progressDialog.setTitle("Loading");
        progressDialog.setCancelable(false);

        request = new File(getExternalCacheDir() + "/request.json");
        response = new File(getExternalCacheDir() + "/response.json");

        sp = getSharedPreferences("login", MODE_PRIVATE);
        spEdit = sp.edit();

        if (!sp.getString("username", "").isEmpty()) {
            editTextUsername.setText(sp.getString("username", ""));
            editTextPassword.setText(sp.getString("password", ""));
        }

        /*
        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());
        */

        buttonLogin.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                progressDialog.setMessage("Logging in");
                progressDialog.show();

                DataPack.LoginRequest loginRequest = new DataPack.LoginRequest();
                loginRequest.username = username = editTextUsername.getText().toString();
                loginRequest.password = password = editTextPassword.getText().toString();
                loginRequest.request = "login";

                spEdit.putString("username", username);
                spEdit.putString("password", password);
                spEdit.apply();

                try {
                    if (request.exists()) request.delete();
                    request.createNewFile();

                    FileOutputStream requestStream = new FileOutputStream(request);
                    requestStream.write(new Gson().toJson(loginRequest).getBytes());
                    requestStream.close();

                    new FTPTask(response, request) {
                        @Override
                        void onPostExecute() {
                            super.onPostExecute();
                            try {
                                progressDialog.dismiss();
                                String responseContent = Util.getContent(response);
                                if (responseContent.equals("")) {
                                    Snackbar.make(buttonLogin, "FTP connection failed! ", Snackbar.LENGTH_LONG).show();
                                }

                                DataPack.LoginResponse loginResponse = new Gson().fromJson(responseContent, DataPack.LoginResponse.class);
                                if (loginResponse.authority.equals("unauthorised")) Snackbar.make(buttonLogin, "Unauthorised! ", Snackbar.LENGTH_LONG).show();
                                else {
                                    Intent intent = new Intent(MainActivity.this, RecordActivity.class);
                                    intent.putExtra("username", username);
                                    intent.putExtra("password", password);
                                    startActivity(intent);
                                }
                            }
                            catch (Exception e) {
                                e.printStackTrace();
                            }
                        }
                    }.execute();
                }
                catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });
    }
}
