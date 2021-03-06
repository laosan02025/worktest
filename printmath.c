#include <stdio.h>
#include <stdlib.h>

#define BUFFERSIZE 256

void main() {
	int i, a, b, c, range, total, calc, piece;//a+b=c
	char tmpbuf[BUFFERSIZE];
	FILE * outfile;
	int format, mask, contiflag, conti, conticalc,sum,match;

	char printformat[BUFFERSIZE];


	outfile = fopen("output.txt", "a+");

	if (!outfile) {
		strcpy(tmpbuf, "打开文件失败");
		goto err;

	}
	printf("输入计算范围:");
	scanf("%d", &range);
	printf("输入算式个数:");
	scanf("%d", &total);
	printf("输入是否连加(1.连加;2.不连加):");
	scanf("%d", &contiflag);

	printf("输入算式格式(1.得数空白;2.算数空白):");
	scanf("%d", &format);

	//contiflag=2;
	if (contiflag == 2) {//不连加
		if (range <= 10)
			strcpy(printformat, "%d %c %d =             ");
		else if (range <= 100)
			strcpy(printformat, "%2d %c %2d =         ");
		else if (range <= 1000)
			strcpy(printformat, "%3d %c %3d =       ");
	}
	else {
		if (format == 1) {
			strcpy(printformat, "%3d %c%3d %c%3d=       ");
		}
	}

	srand(time(NULL));

	for (i = 0; i<total; i++) {
		memset(tmpbuf, 0, BUFFERSIZE);
		if (contiflag == 2) {//不连加
			if (i % 4 == 0) {
				printf("\n");
				fputs("\n", outfile);
			}
		}
		else {//连加
			if (format == 2) {//算数空白
				match = 3;
			}
			else {
				match = 4;
			}
			if (i % match == 0) {
				printf("\n");
				fputs("\n", outfile);
			}
		}

		a = rand() % range;

		calc = rand() % 2;

		if (calc == 0) {//加法
			if ((range - a) != 0) {
				b = rand() % (range - a);
			}
			else {
				b = 0;
			}
		}
		else {//减法
			if (a != 0) {
				b = rand() % a;
			}
			else {
				b = 0;
			}
		}

		//计算得数
		if (calc == 0)//加法
			c = a + b;
		else//减法
			c = a - b;

		if (contiflag == 2) {//不连加
			if (format == 2) {//算数空白
				mask = rand() % 2;//得数空白不处理
				switch (mask) {
				case 0:
					strcpy(printformat, "(   ) %c %2d = %2d       ");
					printf(printformat, (calc ? '-' : '+'), b, c);
					fprintf(outfile, printformat, (calc ? '-' : '+'), b, c);
					break;
				case 1:
					strcpy(printformat, "%2d %c (   ) = %2d       ");
					printf(printformat, a, (calc ? '-' : '+'), c);
					fprintf(outfile, printformat, a, (calc ? '-' : '+'), c);
					break;
				case 2:
					strcpy(printformat, "%2d %c %2d =             ");
					printf(printformat, a, (calc ? '-' : '+'), b);
					fprintf(outfile, printformat, a, (calc ? '-' : '+'), b);
					break;
				default:
					break;
				}
			}
			else {//得数填空
				printf(printformat, a, (calc ? '-' : '+'), b);
				fprintf(outfile, printformat, a, (calc ? '-' : '+'), b);
			}
		}
		else {//连加
			conticalc = rand() % 2;

			if (conticalc == 0) {//加法
				if ((range - c) != 0) {
					conti = rand() % (range - c);
				}
				else {
					conti = 0;
				}
				sum = c + conti;
			}
			else {//减法
				if (c != 0) {
					conti = rand() % c;
				}
				else {
					conti = 0;
				}
				sum = c - conti;
			}
			if (format == 1) {//得数空白
				printf(printformat, a, (calc ? '-' : '+'), b, (conticalc ? '-' : '+'), conti);
				fprintf(outfile, printformat, a, (calc ? '-' : '+'), b, (conticalc ? '-' : '+'), conti);
			}
			else {//算数空白
				mask = rand() % 3;//得数空白不处理
				switch (mask) {
				case 0:
					strcpy(printformat, "(   ) %c %2d %c %2d = %2d       ");
					printf(printformat, (calc? '-' : '+'), b, (conticalc? '-' : '+'),conti,sum );
					fprintf(outfile, printformat, (calc ? '-' : '+'), b, (conticalc ? '-' : '+'), conti, sum);
					break;
				case 1:
					strcpy(printformat, "%2d %c (   ) %c %2d = %2d       ");
					printf(printformat, a, (calc ? '-' : '+'), (conticalc ? '-' : '+'), conti, sum);
					fprintf(outfile, printformat, a, (calc ? '-' : '+'), (conticalc ? '-' : '+'), conti, sum);
					break;
				case 2:
					strcpy(printformat, "%2d %c %2d %c (   ) = %2d       ");
					printf(printformat, a, (calc ? '-' : '+'), b, (conticalc ? '-' : '+'), sum);
					fprintf(outfile, printformat, a, (calc ? '-' : '+'), b, (conticalc ? '-' : '+'), sum);
					break;
				default:
					break;
				}
			}
		}
	}
	printf("\n");
	fputs("\n", outfile);
	fclose(outfile);
	return;
err:
	printf("程序出错:%s", tmpbuf);
}
