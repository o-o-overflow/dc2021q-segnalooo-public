from ubuntu:18.04

run apt-get -qq update
run apt-get install -qq --no-install-recommends xinetd rsyslog software-properties-common
run apt-get install -y netcat-traditional htop expect

run chmod o-w /var/tmp
run chmod o-w /dev/shm

copy flag /flag
copy service.conf /service.conf
copy banner_fail /banner_fail
copy wrapper /wrapper
copy stub /stub

expose 4321
cmd ["/usr/sbin/xinetd", "-syslog", "local0", "-dontfork", "-f", "/service.conf"]

