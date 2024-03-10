#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv) {
    char *fname = argc > 1 ? argv[1] : "test.txt"; // 파일 이름을 인자로 받거나, 기본값 사용
    int fd = open(fname, O_RDONLY); // 파일 열기
    if (fd < 0) {
        perror(fname); // 파일 열기 실패 시 오류 메시지 출력
        exit(EXIT_FAILURE);
    }

    // 파일의 크기를 측정
    off_t fileSize = lseek(fd, 0, SEEK_END);
    if (fileSize == -1) {
        perror("Error seeking file");
        exit(EXIT_FAILURE);
    }

    // 파일 포인터를 다시 파일의 시작 위치로 이동
    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("Error resetting file pointer to the start");
        exit(EXIT_FAILURE);
    }

    // 파일 크기만큼의 메모리 할당
    char *buffer = (char *)malloc(fileSize + 1); // +1은 NULL 문자를 위한 공간
    if (!buffer) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    // 파일의 내용을 메모리에 할당된 배열에 읽어들임
    ssize_t bytesRead = read(fd, buffer, fileSize);
    if (bytesRead < 0) {
        perror("Error reading file");
        free(buffer); // 에러 발생 시 할당된 메모리 해제
        exit(EXIT_FAILURE);
    }

    buffer[bytesRead] = '\0'; // NULL 문자로 문자열 종료

    // 파일 내용 처리 (예시)
    printf("File content:\n%s\n", buffer);

    // 사용 후 파일과 메모리 자원 정리
    close(fd);
    free(buffer);

    return EXIT_SUCCESS;
}
