using System.Data.Entity;
using System.Data.Entity.Migrations;
using IW5Project.Model.Movies;
using IW5Project.Model.Persons;
using IW5Project.Model.Ratings;

namespace IW5Project.Services
{
    public class MoviesDbContext : DbContext
    {
        public DbSet<Person> Persons { get; set; }
        public DbSet<Movie> Movies { get; set; }
        public DbSet<PersonMovieRole> PersonMovieRoles { get; set; }
        public DbSet<Rating> Ratings { get; set; }

        public MoviesDbContext() : base("MoviesDB")
        {
        }

        protected override void OnModelCreating(DbModelBuilder modelBuilder)
        {
            Database.SetInitializer(
                new MigrateDatabaseToLatestVersion<MoviesDbContext, MoviesDbMigrationsConfiguration<MoviesDbContext>>());
            base.OnModelCreating(modelBuilder);
            modelBuilder.Entity<Person>().ToTable("Persons");
            modelBuilder.Entity<Movie>().ToTable("Movies");
            modelBuilder.Entity<PersonMovieRole>().ToTable("PersonMovieRoles");
        }
    }
}