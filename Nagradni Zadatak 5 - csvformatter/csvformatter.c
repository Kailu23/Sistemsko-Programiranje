#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096
#define MAX_COLS 256

int SplitCsvLine(char* line, char* columns[], int maxColumns) {
	int numberOfColumns = 0;
	char* token = strtok(line, ",");
	while (token && numberOfColumns < MAX_COLS) {
		columns[numberOfColumns++] = token;
		token = strtok(NULL, ",");
	}
	return numberOfColumns;
}

int readInputLines(char*** outLines) {
	char line[MAX_LINE];
	char** lines = NULL;
	unsigned int numberOfLines = 0;
	unsigned int linesCapacity = 0;

	while (fgets(line, sizeof(line), stdin)) {
		if (linesCapacity == numberOfLines) {
			linesCapacity = linesCapacity ? linesCapacity * 2 : 16;
			lines = (char**)realloc(lines, linesCapacity * sizeof(char*));
			if (!lines) {
				fprintf(stderr, "Memory error at realloc lines\n");
				return -3;
			}
		}
		lines[numberOfLines] = _strdup(line);
		if (!lines[numberOfLines]) {
			fprintf(stderr, "Memory error at _strdup line\n");
			return -3;
		}
		numberOfLines++;
	}
	*outLines = lines;
	return numberOfLines;
}

int getColumnCountAndAggregateRows(char** lines, int totalLines, int* outDataLines, char** outAverageLine, char** outMinLine, char** outMaxLine) {
	if (totalLines < 3) return 0;

	int dataLines = totalLines - 3;
	*outDataLines = dataLines;
	*outAverageLine = lines[totalLines - 3];
	*outMinLine = lines[totalLines - 2];
	*outMaxLine = lines[totalLines - 1];

	unsigned int numberOfColumns = 0;
	char tmp[MAX_LINE];
	char* columns[MAX_COLS];

	if (dataLines > 0) {
		strcpy(tmp, lines[0]);
		tmp[strcspn(tmp, "\r\n")] = '\0';
		numberOfColumns = SplitCsvLine(tmp, numberOfColumns, MAX_COLS);
	}
	else {
		strcpy(tmp, *outAverageLine);
		tmp[strcspn(tmp, "\r\n")] = '\0';
		numberOfColumns = SplitCsvLine(tmp, columns, MAX_COLS);
	}

	return numberOfColumns;
}

void SplitAggregateRows(char* averageLine, char* minLine, char* maxLine, char* averageColumns[], char* minColumns[], char* maxColumns[]) {
	char tmp[MAX_LINE];

	strcpy(tmp, averageLine);
	tmp[strcspn(tmp, "\r\n")] = '\0';
	SplitCsvLine(tmp, averageColumns, MAX_COLS);

	strcpy(tmp, minLine);
	tmp[strcspn(tmp, "\r\n")] = '\0';
	SplitCsvLine(tmp, minColumns, MAX_COLS);

	strcpy(tmp, maxLine);
	tmp[strcspn(tmp, "\r\n")] = '\0';
	SplitCsvLine(tmp, maxColumns, MAX_COLS);
}

void PrintJSONData(char** lines, int dataLines, int totalColumns) {
	char tmp[MAX_LINE];
	char* dataColumns[MAX_COLS];

	printf("\t\"Data\": [\n");

	for (int i = 0; i < dataLines; i++) {
		strcpy(tmp, lines[i]);
		tmp[strcspn(tmp, "\r\n")] = '\0';
		int totalDataColumns = SplitCsvLine(tmp, dataColumns, MAX_COLS);

		printf("\t\t{");
		for (int c = 0; c < totalColumns && c < totalDataColumns; c++) {
			printf("\"Column%d\": \"%s\"", c + 1, dataColumns[c]);
			if (c < totalColumns - 1) printf(", ");
		}
		printf("}");
		if (i < dataLines - 1) printf(", ");
		printf("\n");
	}
	printf("\t],\n");
}

void PrintJSONAggregate(int totalColumns, char* averageColumns[], char* minColumns[], char* maxColumns[]) {
	printf("\t\"Aggregates\": {\n");
	for (int c = 0; c < totalColumns; c++) {
		const char* averageValue = (c < totalColumns) ? averageColumns[c] : "";
		const char* minValue = (c < totalColumns) ? minColumns[c] : "";
		const char* maxValue = (c < totalColumns) ? maxColumns[c] : "";

		printf("\t\"Column%d\": { \"average_or_most_frequent\": \"%s\", \"minimum\": \"%s\", \"maximum\": \"%s\" }", c + 1, averageValue, minValue, maxValue);
		if (c < totalColumns) printf(",");
		printf("\n");
	}
	printf("\t}\n");
}


int main(void) {
	printf("\t\"Data\": [\n");
	char** lines = NULL;
	unsigned int totalLines = readInputLines(&lines);
	if (totalLines < 0) {;
		return 1;
	}

	if (totalLines < 3) {
		for (int i = 0; i < totalLines; i++) free(lines[i]);
		free(lines);
		return -3;
	}

	int dataLines;
	char* averageLine;
	char* minLine;
	char* maxLine;

	int totalColumns = getColumnCountAndAggregateRows(lines, totalLines, &dataLines, &averageLine, &minLine, &maxLine);

	if (totalColumns <= 0) {
		for (int i = 0; i < totalLines; i++) free(lines[i]);
		free(lines);
		return 0;
	}

	char* averageColumns[MAX_COLS];
	char* minColumns[MAX_COLS];
	char* maxColumns[MAX_COLS];

	SplitAggregateRows(averageLine, minLine, maxLine, averageColumns, minColumns, maxColumns);
	printf("{\n");
	PrintJSONData(lines, dataLines, totalColumns);
	PrintJSONAggregate(totalColumns, averageColumns, minColumns, maxColumns);
	printf("}\n");

	for (int i = 0; i < totalLines; i++) free(lines[i]);
	free(lines);
	return 0;
}