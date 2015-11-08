! Demonstration a few new features of Fortran 2008 with OpenMP
!
! This program load MIV 3D volume into memory, apply mean filter
! and save result back to MIV file format.
program miv_filter
  use omp_lib
  implicit none
  character(len=256) :: sfn, dfn, ksize
  integer :: argc
  integer :: xdim, ydim, zdim, z1, z2, zload, nt, id
  integer, dimension(3) :: vdim
  character, dimension(:,:,:), allocatable :: vol, vfv
  real(kind=8), dimension(3) :: vsize
  integer :: vunit, r = 1
  logical :: ready = .false.
  
  argc = command_argument_count()
  if (argc > 0) then
    call get_command_argument(1, sfn)
    if (len_trim(sfn) > 0) then
      if (miv_load(trim(sfn), vol, vdim, vsize, vunit) /= 0) stop
      ready = .true.
    endif
  endif

  if (.not. ready) then
    print *, "Usage:"
    print *, "  miv_filter src.miv dst.miv kernel_size"
    stop
  endif

  if (argc > 1) then
    call get_command_argument(2, dfn)
  else
    dfn = 'out.miv'
  endif

  if (argc > 2) then
    call get_command_argument(3, ksize)
    read(ksize,'(I4)') r
    r = r / 2
  endif

  xdim = vdim(1); ydim = vdim(2); zdim = vdim(3)
  allocate(vfv(xdim,ydim,zdim))

  write(*,'("Dimensions: ",I0," x ",I0," x ",I0)') vdim
  write(*,'("Voxel size: [",F4.2,",",F4.2,",",F4.2,"]")') vsize
  write(*,'("Kernel radius = ",I0)') r

  nt = omp_get_max_threads()
  zload = (zdim + nt - 1) / nt
!$omp parallel default(shared) private(z1,z2,id)
  id = omp_get_thread_num()
  z1 = id * zload + 1; z2 = z1 + zload - 1
  if (z2 > zdim) z2 = zdim
  write(*,'(I0," => ",I4," - ",I4)') id, z1, z2
  call mean_filter(vol, vfv, xdim, ydim, zdim, z1, z2, r)
!$omp end parallel

  write(*,'("Saving filter result to ",(A))') trim(dfn)
  call miv_save(dfn, vfv, vdim, vsize, vunit)
  deallocate(vol,vfv)

contains
!----------------------------------------------------------------------
subroutine mean_filter(vol, vfv, xdim, ydim, zdim, z1, z2, r)
  character, dimension(:,:,:) :: vol
  character, dimension(:,:,:) :: vfv
  integer :: xdim, ydim, zdim, z1, z2, r
  integer :: x, y, z, dx, dy, dz, kx, ky, kz
  integer :: s, t
  do z = z1, z2
    do y = 1, ydim
      do x = 1, xdim
        s = 0
        t = 0
        do dz = -r, r
          kz = z + dz
          if (kz <= 0 .or. kz > zdim) cycle
          do dy = -r, r
            ky = y + dy
            if (ky <= 0 .or. ky > ydim) cycle
            do dx = -r, r
              kx = x + dx
              if (kx <= 0 .or. kx > xdim) cycle
              s = s + iachar(vol(kx,ky,kz))
              t = t + 1
            enddo ! dx
          enddo ! dy
        enddo ! dz
        vfv(x,y,z) = achar(s/t)
      enddo ! x
    enddo ! y
  enddo ! z
end subroutine
!----------------------------------------------------------------------
function miv_load(fn, vol, vdim, vsize, vunit) result(ret)
  character(len=*), intent(in) :: fn
  character, dimension(:,:,:), intent(out), allocatable :: vol
  integer(kind=4), dimension(3), intent(out) :: vdim
  real(kind=8), dimension(3), intent(out) :: vsize
  integer, intent(out) :: vunit
  character(len=4) :: sig
  character(len=8) :: cl
  integer :: dt, nc
  integer :: ioerr = 0
  integer :: ret

  open(UNIT=100, FILE=fn, FORM='unformatted', STATUS='old', &
       ACTION='read', ACCESS='stream', IOSTAT=ioerr)

  if (ioerr /= 0) then
    print *, 'Cannot open file ', fn, ioerr
    ret = -1
    return
  endif

  read(100) sig
  if (sig /= 'MIVF') then
    print *, fn, ' is not a MIV file'
    close(100)
    ret = -2
    return
  endif
  read(100) vdim
  read(100) dt, vunit
  read(100) vsize
  read(100) nc, cl
  allocate(vol(vdim(1),vdim(2),vdim(3)))
  read(100) vol
  close(100)

  ret = 0
end function
!----------------------------------------------------------------------
subroutine miv_save(fn, vol, vdim, vsize, vunit)
  character(len=*), intent(in) :: fn
  character, dimension(:,:,:), intent(in) :: vol
  integer(kind=4), dimension(3), intent(in) :: vdim
  real(kind=8), dimension(3), intent(in) :: vsize
  integer, intent(in) :: vunit

  integer :: dt = 0, nc = 0, i, ioerr = 0
  character, dimension(8) :: cl = [(achar(0),i=1,8)]

  open(UNIT=100, FILE=fn, FORM='unformatted', STATUS='replace', &
       ACTION='write', ACCESS='stream', IOSTAT=ioerr)

  if (ioerr /= 0) then
    print *, 'Cannot create file ', fn, ioerr
    return
  endif

  write(100) 'MIVF'
  write(100) vdim
  write(100) dt, vunit
  write(100) vsize
  write(100) nc, cl
  write(100) vol
  close(100)
end subroutine
!----------------------------------------------------------------------
end
