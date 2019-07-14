package cc.hurrypeng.intellieco;

import android.os.Handler;
import android.os.Message;

import androidx.annotation.NonNull;

import org.apache.commons.net.ftp.FTP;
import org.apache.commons.net.ftp.FTPClient;
import org.apache.commons.net.ftp.FTPReply;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.UUID;

class FTPTask extends Thread {
    final static int RESPONSE_RECV = 1;

    protected File response;
    protected ArrayList<File> sends;

    protected Handler handler;

    protected String server;
    protected String ftpUsername;
    protected String ftpPassword;
    protected String uuid;

    FTPTask(File response, File... send) {
        super();
        this.response = response;
        sends = new ArrayList<File>();
        for (File file : send) sends.add(file);

        handler = new FTPTaskHandler();

        server = "ie.hurrypeng.cc";
        ftpUsername = "IE_User";
        ftpPassword = "PKUSZE410";
        uuid = UUID.randomUUID().toString().replaceAll("-", "");
    }

    void execute() {
        onPreExecute();
        start();
    }

    void onPreExecute() {
    }

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
                return;
            }

            if(!ftp.login(ftpUsername, ftpPassword)) {
                return;
            }

            ftp.enterLocalPassiveMode();
            ftp.setRemoteVerificationEnabled(false);

            ftp.makeDirectory(uuid);
            ftp.changeWorkingDirectory(uuid);
            ftp.setFileType(FTP.BINARY_FILE_TYPE);

            for (File file : sends) {
                String filename = file.getName();
                ftp.storeFile(filename, new FileInputStream(file));
            }

            ftp.makeDirectory("request");

            // Wait for response
            while (true)
            {
                if (ftp.removeDirectory("response")) break;

                try {
                    Thread.sleep(1000);
                }
                catch (Exception e) {
                    e.printStackTrace();
                }
            }

            if (response.exists()) response.delete();
            response.createNewFile();

            FileOutputStream responseStream = new FileOutputStream(response);
            ftp.retrieveFile("response.json", responseStream);
            responseStream.close();

            ftp.makeDirectory("receive");

            ftp.disconnect();

            Message message = new Message();
            message.what = RESPONSE_RECV;
            handler.sendMessage(message);

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    class FTPTaskHandler extends Handler {
        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);

            switch (msg.what) {
                case FTPTask.RESPONSE_RECV: {
                    onPostExecute();
                    break;
                }
            }
        }
    }

    void onPostExecute() {
    }
}
