#define _CRT_SECURE_NO_WARNINGS 1

// �ҵ��������⣺
// 1. ����һ�����飬ֻ��һ��Ԫ��ֻ������һ�Σ�����Ԫ�ض����������Σ����ҳ����Ԫ�ء�
// 2. ���������������Ԫ��ֻ������һ�Σ����������������Σ����ҳ�������Ԫ�ء�

// ���������ⶼӦ�õ��˰�λ���
// ���������ͬ������λ�����Ϊ��
// ������ǰ��� ����ƽ��ֵ�� ���������������� ������λ �� ��ͬ��λ ��ʾ������ ��


#include<stdio.h>

int FindOneSingleDog(int arr[], int size) {

	int Dog = arr[0];

	for (int i = 1; i < size; i++) {
		Dog = Dog ^ arr[i];
	}

	return Dog;
}

void FindTwoSingleDog(int arr[], int size, int* Dog1, int* Dog2) {

	int rst = arr[0];
	int leftPos = 0;

	for (int i = 1; i < size; i++) {
		rst = rst ^ arr[i];
	}
	//��ʱ rst �ȼ��� Dog1 ^ Dog2, �������ı���λһ����һ���� 1

	// �ҵ���һ���� 1 �ı���λ
	for (leftPos = 0; leftPos < 32; leftPos++) {
		if ((rst & (1 << leftPos)) == 1) {
			break;
		}
	}

	// ������Ԫ�طֳ������֣�һ���� leftPos λΪ 1����һ���ֲ�Ϊ 1
	// ��ô Dog1 �� Dog2 ���ڲ�ͬ�������ˡ��ٴΰ�λ��򣬽������ Dog1 �� Dog2 ��ֵ

	*Dog1 = 0;
	*Dog2 = 0;

	for (int i = 0; i < size; i++) {
		// leftPos Ϊ 1 ����
		if ((arr[i] & (1 << leftPos)) == 1) {
			*Dog1 = *Dog1 ^ arr[i];
		}
		else {
			*Dog2 = *Dog2 ^ arr[i];
		}
	}


}

int main(void) {

	int arr1[] = { 1, 2, 3, 4, 1, 2, 3 };
	int size1 = sizeof(arr1) / sizeof(arr1[0]);
	int Dog;

	int arr2[] = { 1, 1, 2, 2, 3, 4 };
	int size2 = sizeof(arr2) / sizeof(arr2[0]);
	int Dog1, Dog2;

	Dog = FindOneSingleDog(arr1, size1);

	FindTwoSingleDog(arr2, size2, &Dog1, &Dog2);

	return 0;
}

