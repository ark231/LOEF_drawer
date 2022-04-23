function(scoped_message mode msg) #ビルトインのmessageはモードはオプショナルだが、実装が難しそうなので……
    list(APPEND ARGN ${msg})
    list(JOIN ARGN "" JOINED_MESSAGE)
    message(${mode} "${PROJECT_NAME}: ${JOINED_MESSAGE}")
endfunction()
