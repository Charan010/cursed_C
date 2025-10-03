#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

/*

    lets say you have get tons of metrics data of maybe cpu temperature. generally excluding some anomalies
    most of the data variance would be pretty less meaning data is pretty near to each other.

    so instead of naively storing lets say 80,82,83,84 values each taking up to 4bytes for integer representation.
    take variance of these meaning 80 ,2, 1, 1 and instead of using 4 bytes use 2 bytes for storing these difference values.


    you can use 7 bits from each byte, if MSB = 1, then there is still one more byte which needs to be decoded. Lets say i want to encode 132 value
    then you can put 128 in first byte and 132-128 = 4 value in 2nd byte.

       4   +  128 
    00000100 , 11000000 (using little endian representation)

    This way if the variance is pretyy less then you can reduce 75% of byte size . but the main tradeoff that we are willing to do
    is that you cant expect random access because you need to sequentially decode values from starting so
    much more useful for analysis stuff.

*/


//zigzag to handle negative values when computing difference as MSB bit in varint used to indicate if any other bytes exists or not . not for significant bit anymore
int32_t zigzag_encode(int32_t x){
    return (x << 1) ^ (x >> 31);
}

int32_t zigzag_decode(uint32_t n) {
    return (n >> 1) ^ -(n & 1);
}

int encode_varint(uint32_t value, uint8_t *out){
    int i = 0;
    
    //NOTE: 0x80 = 128, so while value is greater than 128 which is 7 bits
    while(value >= 0x80){
        
        //copy all 7 bits and put MSB as 1 , as you are pushing more bits ahead and shr 7 bits
        out[i++] = (value & 0x7F) | 0x80;
        value >>= 7;
    }

    out[i++] = value & 0x7F;
    return i;
}

int decode_varint(const uint8_t *in , uint32_t *value){

    int i = 0, shift = 0;
    uint32_t result = 0;

    while(1){
        uint8_t byte = in[i++];

        //take 7 bits , compute and add them to result and shift right to 7 bits to decode next byte
        result |= ((uint32_t)(byte & 0x7F) <<  shift);
         if (!(byte & 0x80))
            break; 
        shift += 7;

    }

    *value = result;
    return i;
}

int main() {

    srand(time(NULL));

    int n = 100000;
    uint32_t nums[n];

    nums[0] = 1000;

    for(int i = 1 ; i < n ; ++i)
        nums[i] = nums[i-1] + (rand()%5); 
    
    FILE *f1 = fopen("naive.bin", "wb");
    if(!f1){
        perror("fopen");
        return 1;
    }

    for(int i = 0 ; i < n ; ++i){
        fwrite(&nums[i],sizeof(uint32_t), 1 ,f1);
    }
    fclose(f1);

    f1 = fopen("naive.bin","rb");
    fseek(f1,0,SEEK_END);
    long naive_size = ftell(f1);
    fclose(f1);
    printf("Naive bytes: %ld\n", naive_size);

    FILE *f2 = fopen("varint.bin", "wb");
    if(!f2){
        perror("fopen");
        return 1;
    }

    uint32_t prev = 0;
    for(int i = 0 ; i < n ; ++i){
        int32_t delta;
        if(i == 0)
            delta = (int32_t)nums[0];
        else
            delta = (int32_t)(nums[i] - prev);

        uint32_t zz = zigzag_encode(delta);
        uint8_t buf[10];
        int bytes = encode_varint(zz, buf);
        fwrite(buf, 1, bytes, f2);
        prev = nums[i];
    }

    fclose(f2);

    f2 = fopen("varint.bin","rb");
    fseek(f2,0,SEEK_END);
    long varint_size = ftell(f2);
    fclose(f2);
    printf("Varint bytes: %ld\n", varint_size);

    return 0;
}