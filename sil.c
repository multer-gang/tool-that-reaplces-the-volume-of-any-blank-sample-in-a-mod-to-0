/*
 * tool to silence blank samples in a MOD file
 * there's no checks here, so be careful with what files you use.
 */

#include <stdio.h>
#include <stdlib.h>

static size_t file_size = 0;

void silence_blank_sample(char* header)
{
	size_t length = 0;
	if (header == NULL)
		return;

	length = (header[23] | (header[22] << 8));
	if (length == 0)
		header[25] = 0;
}

char* read_file_into_buffer(char* filename)
{
	FILE* f = NULL;
	char* file_buf = NULL;
	size_t success_read = 0;
	if (filename == NULL)
		return NULL;
	f = fopen(filename, "rb");
	if (!f)
	{
		perror("fopen");
		return NULL;
	}
	fseek(f, 0, SEEK_END);
	file_size = (size_t)ftell(f);
	rewind(f);
	file_buf = (char*)malloc(file_size);
	if (!file_buf)
	{
		perror("malloc");
		return NULL;
	}
	success_read = fread(file_buf, 1, file_size, f);
	if (success_read != file_size)
	{
		fprintf(stderr, "didn't read the whole file...\n");
		goto error_happeneded;
	}
	if (feof(f))
	{
		fprintf(stderr, "unexpected EOF!\n");
		goto error_happeneded;
	}
	else if (ferror(f))
	{
		fprintf(stderr, "something went wrong with the stream!\n");
		goto error_happeneded;
	}
	fclose(f);
	return file_buf;
error_happeneded:
	if (f != NULL)
		fclose(f);
	if (file_buf != NULL)
		free(file_buf);
	return NULL;
}

int main(int argc, char** argv)
{
	char* file_buf = NULL;
	size_t i = 0;
	if (argc < 2)
	{
		printf("usage: %s module.mod\n", argv[0]);
		goto error_happened;
	}

	file_buf = read_file_into_buffer(argv[1]);
	if (!file_buf)
	{
		goto error_happened;
	}

	for (i = 0; i < 31; i++)
	{
		silence_blank_sample(&file_buf[20 + (i * 30)]);
		printf("handling sample %d\n", (int)(i + 1));
	}

	f = fopen(argv[1], "wb");
	if (!f)
	{
		perror("fopen");
		goto error_happened;
	}
	fwrite(file_buf, 1, file_size, f);
	fclose(f);

	if (file_buf != NULL)
		free(file_buf);

	return EXIT_SUCCESS;

error_happened:
	if (file_buf != NULL)
		free(file_buf);
	return EXIT_FAILURE;
}