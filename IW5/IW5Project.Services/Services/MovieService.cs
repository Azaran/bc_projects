using IW5Project.Model.Movies;
using IW5Project.Services.Repository;

namespace IW5Project.Services.Services
{
    public class MovieService : Repository<MoviesDbContext, Movie>
    {
    }
}
