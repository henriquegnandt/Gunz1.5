REM cxr���ϵ��� ������Ϸ� ���ڵ��ϴ� ��ġ�����Դϴ�.
REM �� ��ġ������ ������ �̺�Ʈ�� ����ǵ��� �մϴ�.
REM ���ڵ��� ������ �̸��� cxr_ ���ξ ���̱�� �մϴ�.
REM ������ϵ��� ��� ������ �н����带 ����ؾ� �Ѵٴ����� �����ϼ���. (���ڴ��� �����ϱ� ������)
REM .

SET CXRPASSWORD=eJh^5ihjB*)uQB(#]($1;vx

cxr -i	test.cxr		-o cxr_test.h			-p %CXRPASSWORD%
cxr -i MMatchItem.cxr	-o cxr_MMatchItem.h		-p %CXRPASSWORD%
cxr -i MMatchBuff.cxr	-o cxr_MMatchBuff.h		-p %CXRPASSWORD%
pause
