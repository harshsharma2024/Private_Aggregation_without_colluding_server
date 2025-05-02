user_prg_time = [192.607,195,201.993,195.626,197.486]
user_mask_gneration_time = [198.329, 201,207.886,201.343,203.586]
user_network_time = [1014.06, 1255,1246.74,1426.17,1245.578]
user_mask_eval_time = [0, 0, 0,0,0]
user_pk_lst_extr_time = [0, 0, 0,0,0]
user_tot_time = [1274, 1515, 1529, 1696,1514]

agg_network_time = [2642, 4120, 4659, 3712, 3641]
agg_poly_agg_time = [0,0, 0, 0, 0]
agg_mask_process_time = [0,0,0,0,0]
agg_tot_time = [2889, 4374, 4919, 3964,3895]

import matplotlib.pyplot as plt

avg_user_prg_time = sum(user_prg_time) / len(user_prg_time)
avg_user_mask_gneration_time = sum(user_mask_gneration_time) / len(user_mask_gneration_time)
avg_user_network_time = sum(user_network_time) / len(user_network_time)
avg_user_mask_eval_time = sum(user_mask_eval_time) / len(user_mask_eval_time)
avg_user_pk_lst_extr_time = sum(user_pk_lst_extr_time) / len(user_pk_lst_extr_time)
avg_user_tot_time = sum(user_tot_time) / len(user_tot_time)

avg_agg_network_time = sum(agg_network_time) / len(agg_network_time)
avg_agg_poly_agg_time = sum(agg_poly_agg_time) / len(agg_poly_agg_time)
avg_agg_mask_process_time = sum(agg_mask_process_time) / len(agg_mask_process_time)
avg_agg_tot_time = sum(agg_tot_time) / len(agg_tot_time)

# plot bar graph for user and aggregator times
user_labels = ['PRG_time', 'mask_gen_time', 'network_time', 'mask_eval', 'pk_lst_extr', 'tot_time']
user_times = [avg_user_prg_time, avg_user_mask_gneration_time, avg_user_network_time, avg_user_mask_eval_time, avg_user_pk_lst_extr_time, avg_user_tot_time]
agg_labels = ['network_time', 'poly_agg_time', 'mask_process_time', 'tot_time']
agg_times = [avg_agg_network_time, avg_agg_poly_agg_time, avg_agg_mask_process_time, avg_agg_tot_time]
x = range(len(user_labels))
agg_x = range(len(agg_labels))
plt.figure(figsize=(24, 12))
plt.subplot(1, 2, 1)
plt.bar(x, user_times, color='blue', alpha=0.7)
plt.xticks(x, user_labels)
plt.ylabel('Time (milli seconds)')
plt.title('User Times')

plt.subplot(1, 2, 2)
plt.bar(agg_x, agg_times, color='orange', alpha=0.7)
plt.xticks(agg_x, agg_labels)
plt.ylabel('Time (milli seconds)')
plt.title('Aggregator Times')
plt.tight_layout()
plt.show()