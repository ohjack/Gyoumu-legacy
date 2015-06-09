<?php
namespace Ebapi\API;

module_load_include('php', 'ebapi', 'inc/Services_Ebay/Ebay');

class Helper
{
  private $sandbox;

  public function __construct($sandbox)
  {
    $this->sandbox = $sandbox;
  }

  public function getRuName()
  {
    if($this->sandbox){
      return 'Maolung-Maolung71-9e40--cqotxnjz';
    }
    return 'Maolung-Maolungf1-057f--xwubahhpm';
  }

  public function getSession()
  {
    if($this->sandbox){
      $devId  = 'ebfad531-903c-41e4-b1af-d16369d81e75';
      $appId  = 'Maolung71-9e40-49c5-9a37-aa903b58a1b';
      $certId = '6cd3588e-d011-4618-96c6-c251c47f99c7';
      //    $token = 'AgAAAA**AQAAAA**aAAAAA**nwiMTg**nY+sHZ2PrBmdj6wVnY+sEZ2PrA2dj6wFk4GhCJCGpQSdj6x9nY+seQ**ASkBAA**AAMAAA**h2S/tokAFDJfAa0TuSHEYcXxVS4zN2FlxDBfXegfjJQ28h4bp6C6jYGXpa6KXTq1xfaw7dOdZYy2XuLaVngFN11Lj3aHjep6HMnvEye3ncgka6xRAmqcSX4SEavH1mBWhae7INI5YeIWBD0aYPicoC+Z6U+PXyOC6s9AWvQkNOxn1xhWen7oWJx20uETOSBUw/Bt4Y1ZuWCuBx5jkIgGEWw0GKyK9wwvCYGVeM6tfeFRt92K2iWsYvYoiWznfzEgpmpCtVQfjchiYJ+Lr+fJFvdJesiREPJJ5r40PGZSZUgeYeAVESme0UVdAqdqFiHKcFMg27UpRCgyNhRbINfWxwX6QLqWQrIm7xVlIVSQY5B5gPLr5guTLMSGOIVgbq+yRMCVd32avQGu5q7T4lZ6wxM7S76qhR15S49bD2RSleG3TaS2fqplWA9oEKhA5g17X7wn+qkRdPkNDmtbL+ZYp2pOH6K8saiLhuKvXtx78knpW555x79T/miqw7cXtMtHdHFq+zjGOgSvTuaYgtaSsDpxJcCsIO/7Qq/twYfyKIEGskXznguFcIQQ9PzeL/dBBWoPCLe9D3mSd/5/J9vXxv+OXYI5lhzFyy2BGfULFHNWrA7W8dc28V0F62E4fMYgK51okpOACVkxfGy6vqDEq3ach6EL8KJOYspv4TLL/zdTODQ6cUxccGmez2+55disfmrnZjeSJrFyw41LIiBXcPv8HzgFGYnJ6Z11Yqf59Ai4ImewhyNEN/1YqJLpbgH+';
      return \Services_Ebay::getSandboxSession($devId, $appId, $certId);
    }

    $devId  = 'ebfad531-903c-41e4-b1af-d16369d81e75';
    $appId  = 'Maolungf1-057f-4fba-b6ef-a4799e15da0';
    $certId = 'c316d544-82e1-4535-bf46-f4aa59a738f5';
    //    $token = 'AgAAAA**AQAAAA**aAAAAA**hkuqTg**nY+sHZ2PrBmdj6wVnY+sEZ2PrA2dj6wNmYKmAJGLogudj6x9nY+seQ**IksBAA**AAMAAA**l9od22Wv9AGV6Q40/rJj/AqJcvmokLvxcT6c6dWDCzj0wjUgFXoPX0XITeJ2lmkoNbr43s5NlFl2yYXa6IU5rksbHo3Ia2epetkee1DfrzdHNSfs/S+fh3+GMXsQXlXVpEu+urcdHk/MHQ1i+RF7/nQzmvIWl+koY9U/d58Cga56rJ2Ccm0dBpqEEavJFEOTrLr3FD5HljXKATIcmpS3cYmDw/vbnMjyhhDxbQVt40mrKxQvV/5ONma/AgByuTGLik0ox/Mamaf8K4SDzT1HGYM6rjDFica47/Agt+lVyKS133S8ol19d40JTtVnRzGAX7+qhFTSg4fcrcB8kHZe9jaNDsXYlh0GJUlrG7oKvXCyGZWJsjll/QADdbNv51Wujtzz6A0rd804gsbYdyGphgPzbtAbJzpiK6ZrPposUDe8oFcb3tYIMVlxWYRevIo33ZpuXQGvPeaoQhWx3yMiFg+oXsKHYNNUJX9AmjWtqzY2lFkn4Pu4+fTAFCVTZYrT/G0wDXHU3Q3eiE3eZd39zE+yF1xv6n6GHA3upzyoomxESKx2F8XqSNmt03K4P9Rix0Jq8kT11MHIUKo4VRJ8eVac48XEw26VWc8rLye5Vi+01ZH9SJcSKCFAPFErH7SsfZ9nbkfrNju42S+YcsF+llUHg92a0OrLW6UEIbonseHNylla8HpirpbjHrxrwA3sJC7qV+8SyWuM2L8DeAPPE7CQ1DJrV8nXmVLtqM1rbRlAnKVSZWhAMaNiy93/5R6E';
    return \Services_Ebay::getSession($devId, $appId, $certId);
  }

  private function getQuery()
  {
    return new \Services_Ebay($this->getSession());
  }

  public function getSessionID()
  {
    $q = $this->getQuery();
    return $q->GetSessionID($this->getRuName());
  }

  public function getSignLink($sid)
  {
    $tpl = $this->sandbox
        ? "https://signin.sandbox.ebay.com/ws/eBayISAPI.dll?SignIn&RuName=%s&SessID=%s"
        : "https://signin.ebay.com/ws/eBayISAPI.dll?SignIn&RuName=%s&SessID=%s";

    return sprintf($tpl, $this->getRuName(), $sid);
  }

  public function fetchToken($sid)
  {
    $q = $this->getQuery();
    return $q->FetchToken($sid);
  }
}