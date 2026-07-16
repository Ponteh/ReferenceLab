(() => {
  const year = document.querySelector('#year');
  if (year) year.textContent = new Date().getFullYear().toString();

  const pagesHost = window.location.hostname.match(/^([^.]+)\.github\.io$/i);
  const pathParts = window.location.pathname.split('/').filter(Boolean);

  document.querySelectorAll('.repo-link').forEach((link) => {
    if (pagesHost && pathParts.length > 0) {
      link.href = `https://github.com/${pagesHost[1]}/${pathParts[0]}`;
    } else {
      link.href = 'https://github.com/';
    }
  });
})();
