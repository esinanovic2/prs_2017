  __kernel void dot_product(__global float* a_vec, __global float* b_vec, 
						__global float* output, __local float* partial_dot) {

  int global_index = get_global_id(0);
  int local_index = get_local_id(0);
  // Load data into local memory
  partial_dot[local_index] = a_vec[global_index] * b_vec[global_index];
  barrier(CLK_GLOBAL_MEM_FENCE);
  for(int offset = 1;
      offset < get_local_size(0);
      offset <<= 1) {
    int mask = (offset << 1) - 1;
    if ((local_index & mask) == 0) {
      float other = partial_dot[local_index + offset];
      float mine = partial_dot[local_index];
      partial_dot[local_index] = (mine < other) ? mine : other;
    }
    barrier(CLK_LOCAL_MEM_FENCE);
  }
  if (local_index == 0) {
    output[get_group_id(0)] = partial_dot[0];
  }





}