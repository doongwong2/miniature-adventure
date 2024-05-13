#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// 子執行緒函數
void* child(void* arg) {
	int* input = (int*)arg; // 取得資料
	int* result = (int*)malloc(sizeof(int) * 1); // 配置記憶體
	int i = (int)malloc(sizeof(int));
	for ( i = input[0],result[0] = 0; i <= input[1]; i++)
	{
		printf("%d\n", i);
		result[0] += i; // 進行計算
	}

	pthread_exit((void*)result); // 傳回結果
	return 0;
}

// 主程式
int main() {
	pthread_t t,u;
	void* ret; // 子執行緒傳回值
	int input[2] = { 1, 25 }; // 輸入的資料
	int input1[2] = { 26, 50 };

	// 建立子執行緒，傳入 input 進行計算
	pthread_create(&t, NULL, child, (void*)input);
	pthread_create(&u, NULL, child, (void*)input1);
	// 等待子執行緒計算完畢
	pthread_join(t, &ret);

	// 取得計算結果
	int* result = (int*)ret;

	//pthread_join(u, &ret);
	//int* result1 = (int*)ret;

	// 輸出計算結果
	printf("%d %d" ,result[0]);

	// 釋放記憶體
	free(result);

	return 0;
}
