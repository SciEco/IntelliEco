package cc.hurrypeng.intellieco;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.LinearInterpolator;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import java.text.DecimalFormat;
import java.text.SimpleDateFormat;
import java.util.Collection;
import java.util.Collections;
import java.util.Date;
import java.util.List;

class RecordAdapter extends RecyclerView.Adapter {

    private List<DataPack.Record> records;

    static class ViewHolder extends RecyclerView.ViewHolder {

        TextView textViewMothCount;
        TextView textViewRecordInfo;

        public ViewHolder(@NonNull View itemView) {
            super(itemView);
            textViewMothCount = itemView.findViewById(R.id.TextViewMothCount);
            textViewRecordInfo = itemView.findViewById(R.id.TextViewRecordInfo);
        }
    }

    public RecordAdapter(List<DataPack.Record> records) {
        super();
        this.records = records;
        Collections.reverse(this.records);
    }

    @NonNull
    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.record_item, parent, false);
        ViewHolder holder = new ViewHolder(view);
        return holder;
    }

    @Override
    public void onBindViewHolder(@NonNull RecyclerView.ViewHolder holder, int position) {
        ViewHolder holder1 = (ViewHolder) holder;
        DataPack.Record record = records.get(position);
        SimpleDateFormat format=new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        Date date = new Date(record.time);
        String timeString = format.format(date);

        DecimalFormat df = new DecimalFormat("#.0000");

        holder1.textViewMothCount.setText(String.valueOf(record.mothCount));
        holder1.textViewRecordInfo.setText("Uploader: " + record.uploader + "\n" +
                                           " Time: " + timeString + '\n' +
                                           "Location: " + df.format(record.longtitude) + ", " + df.format(record.latitude));
    }

    @Override
    public int getItemCount() {
        return records.size();
    }
}
