Drupal.behaviors.oerp_gallery_approveFixedPictures = function(){
  $('img.gallery').click(function(){
    var $img = $(this);
    var $slot = $img.parents('div.gallery-slot:eq(0)');
    var $note = $slot.find('input.note').parents('div:eq(0)');
    var $chk = $slot.find('input.approve');

    if($chk.attr('checked')){
      $chk.removeAttr('checked');
      $slot.removeClass('checked');
    }
    else if(!$chk.attr('disabled')){
      $chk.attr('checked', 'checked');
      $slot.addClass('checked');
    }
  });

  var selFname = 'div.gallery-fname';

  $(selFname).hover(
      function(){
        $(this).addClass('toDelete');
      },
      function(){
        $(this).removeClass('toDelete');
      }
  );

  $(selFname).click(function(){
    var $slot = $(this).parents('div.gallery-slot:eq(0)');
    $slot.toggleClass('toDelete');

    var $input = $slot.find('div[id$="delete-wrapper"] input');

    if($input.attr('checked')){
      $input.removeAttr('checked');
    }
    else{
      $input.attr('checked', 'checked');
    }
  });
}